from formula import Formula
from utils import vname, get_neighbor_vertices

def convert_to_formula(input_path):
    '''Given an input file, returns a Formula object
    that represents the cell routing instance.
    This encoding does not prevent any extra cycle to appear.
    '''
    # Parse the input, and check its integrity
    from input_parser import parse_input
    parsed_input = parse_input(input_path)
    num_nets   = len(parsed_input['points'])
    num_dims   = parsed_input['num_dims']
    dim_sizes  = parsed_input['dim_sizes']
    # Add all points to a set. This will help us to determine
    # which points are endpoints and which ones are not.
    # These points have different constraints. More precisely,
    # a point that is an endpoint must have exactly one outgoing edge,
    # while a non-endpoint point can have zero or exactly two incident edges
    endpoints = set()
    net_id = 0
    netid = {}
    valid_variables = set()

    def random_routes(s, t, netid, k = 20):
        '''Function that generates some path candidates between two points
        s and t that belong to a net netid.
        The routes are generated as follows:
        1) The d! main "Ls" are added
        2) Generates k random points, and the d! corresponding "Ls"
           between s-k, and between k-t
        Note that 2) may also generate some "unwanted" paths. This is good,
        since it will allow us to consider even more possible paths.
        '''
        def get_ls(s, t):
            '''Function that generates all the Ls between two points s,t
            and adds all the points that belong to the shortest path to a set
            '''
            import itertools
            from copy import deepcopy
            all_perms = itertools.permutations(range(num_dims))
            ret = set()
            for permutation in all_perms:
                start = list(deepcopy(s))
                ret.add('-'.join(str(x) for x in start) + '-' + str(netid))
                for dimension in permutation:
                    while start[dimension] != t[dimension]:
                        if start[dimension] < t[dimension]:
                            start[dimension] += 1
                        else:
                            start[dimension] -= 1
                        ret.add('-'.join(str(x) for x in start) + '-' + str(netid))
            return ret

        ret = get_ls(s, t)
        from random import randint
        for _ in range(k):
            random_point = \
             [randint(0, dim_sizes[i] - 1) for i in range(num_dims)]
            ret |= get_ls(s, random_point) | get_ls(random_point, t)
        return ret

    for (s, t) in parsed_input['points']:
        endpoints.add(s)
        endpoints.add(t)
        netid[s] = net_id
        netid[t] = net_id
        valid_variables |= random_routes(s, t, net_id, max(dim_sizes) // 2)
        net_id += 1

    # Add all grid vertices as variables
    # Internally, all variables that represent a vertex will be of the
    # form coord1-coord2- ... -coordk-net
    # And mean the following: there is a wire from
    # the kth net in the vertex coord1-coord2- ...
    vertices_by_coord = set()
    def add_all_vertices(formula, coordinates, index):
        if index == num_dims:
            coords = '-'.join(str(x) for x in coordinates)
            vertices_by_coord.add(coords)
            for k in range( num_nets ):
                formula.add_variable(coords + \
                '-' + str(k))
        else:
            for i in range( parsed_input['dim_sizes'][index] ):
                coordinates[index] = i
                add_all_vertices(formula, coordinates, index + 1)

    ret = Formula()
    # Basic variables: coord1-coord2-...-coordk-netid
    add_all_vertices(ret, [0] * num_dims, 0)

    # First constraint: we can have at most one of the nets passing
    # through some vertex. That is, if we have 2 nets, then only
    # one of 0-0-0 and 0-0-1 can be true at the same time. Note that
    # they can be all false
    # If the point is and endpoint, then it has exactly one valid
    # net. The rest of them can be directly set to False
    for var in vertices_by_coord:
        point_form = tuple([int(x) for x in var.split('-')])
        if point_form in endpoints:
            # Unit clauses that affirm or negate the appropriate
            # variables. By explicitly setting variables to some value
            # we also speed up a bit the solving process
            for k in range( num_nets ):
                ret.add_clause([(vname(var, str(k)), \
                k == netid[point_form])])
        else:
            # Conventional at most one
            to_add = []
            for k in range( num_nets ):
                if vname(var, str(k)) in valid_variables:
                    to_add.append(vname(var, str(k)))
            ret.at_most_heule([(x, True) for x in to_add], 1)

    # Second constraint: all points, if used and are not endpoints,
    # must have two set neighbors
    # that is: p => at_least_two(adj_p)
    # !p or alt1, !p or alt2, ...
    # This constraint also optimizes a bit the given solutions.
    # For example, the following case cannot happen:
    # S ===
    # T ===
    # It can be proven that if a input has some valid solution,
    # then the restricted set of solutions induced by this constraint
    # will be not empty
    for net in range( num_nets ):
        for v in vertices_by_coord:
            varname = vname(v, str(net))
            if not varname in valid_variables:
                ret.add_clause(
                    (
                        (varname, False),
                    )
                )
            else:
                point_form = tuple([int(x) for x in v.split('-')])
                adjv = [(vname('-'.join(str(y) for y in x), str(net)), True) \
                for x in get_neighbor_vertices(point_form, dim_sizes) if \
                vname('-'.join(str(y) for y in x), str(net)) in valid_variables]
                if point_form in endpoints:
                    if net == netid[point_form]:
                        e = 1
                    else:
                        continue
                else:
                    e = 2
                ret.prepend_implicant((varname, True))
                ret.exactly_heule(adjv, e)
                ret.clear_implicant()

    return (parsed_input, ret)
