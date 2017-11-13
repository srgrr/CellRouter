from formula import Formula
from utils import vname, get_neighbor_vertices

def convert_to_formula(input_path):
    '''Given an input file, returns a Formula object
    that represents the cell routing instance.
    This encoding explicitly prevent unnecessary cycles.
    It emits many unnecessary variables, so it only works for small testcases.
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
    for (s, t) in parsed_input['points']:
        endpoints.add(s)
        endpoints.add(t)
        netid[s] = net_id
        netid[t] = net_id
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
    # Auxiliary variable
    # S-v1-v2: v2 is the successor of v1
    for var in vertices_by_coord:
        point_form = tuple([int(x) for x in var.split('-')])
        adjv = ['-'.join(str(y) for y in x) \
        for x in get_neighbor_vertices(point_form, dim_sizes)]
        for adj in adjv:
            ret.add_variable(vname('S', var, adj))

    # Auxiliary variable
    # U-v: We are using vertex v
    for v in vertices_by_coord:
        ret.add_variable(vname('U', v))

    # Any v-n implies U-v
    for v in vertices_by_coord:
        for net in range( num_nets ):
            ret.add_clause(
                (
                (vname(v, str(net)), False),
                (vname('U', v), True)
                )
            )

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
                to_add.append(vname(var, str(k)))
            ret.at_most([(x, True) for x in to_add], 1)

    # All points, except for sinks, must have exactly one successor
    # If activated--
    # Sinks must have, in fact, zero successors
    processed_nets = set()
    sources = set()

    for var in vertices_by_coord:
        point_form = tuple([int(x) for x in var.split('-')])
        adjv = ['-'.join(str(y) for y in x) \
        for x in get_neighbor_vertices(point_form, dim_sizes)]
        e = 1 if not point_form in endpoints or not netid[point_form] in processed_nets else 0
        #for net in range( num_nets ):
        ret.prepend_implicant((vname('U', var), True))
        ret.exactly([(vname('S', var, x), True) for x in adjv], e)
        ret.clear_implicant()
        if point_form in endpoints and not netid[point_form] in processed_nets:
            processed_nets.add(netid[point_form])
            sources.add(point_form)

    # All points, except for sources, must be the successor of exactly one point
    # If activated
    # Sources must be the successors of zero points
    for var in vertices_by_coord:
        point_form = tuple([int(x) for x in var.split('-')])
        adjv = ['-'.join(str(y) for y in x) \
        for x in get_neighbor_vertices(point_form, dim_sizes)]
        e = not point_form in sources
        #for net in range( num_nets ):
        ret.prepend_implicant((vname('U', var), True))
        ret.exactly([(vname('S', x, var), True) for x in adjv], e)
        ret.clear_implicant()

    # Successors must belong to the same net
    for net in range( num_nets ):
        for var in vertices_by_coord:
            point_form = tuple([int(x) for x in var.split('-')])
            adjv = ['-'.join(str(y) for y in x) \
            for x in get_neighbor_vertices(point_form, dim_sizes)]
            for adj in adjv:
                ret.add_clause(
                    (
                        (vname('S', var, adj), False),
                        (vname(var, str(net)), False),
                        (vname(adj, str(net)), True)
                    )
                )

    # Auxiliary variable
    # R-v1-v2: we can reach v2 from v1
    for v1 in vertices_by_coord:
        for v2 in vertices_by_coord:
            ret.add_variable(vname('R', v1, v2))
    # If B is the successor of A, then A can reach B
    for var in vertices_by_coord:
        point_form = tuple([int(x) for x in var.split('-')])
        adjv = ['-'.join(str(y) for y in x) \
        for x in get_neighbor_vertices(point_form, dim_sizes)]
        for adj in adjv:
            succ = vname('S', var, adj)
            reach = vname('R', var, adj)
            ret.add_clause(
                (
                    (succ, False),
                    (reach, True)
                )
            )
    # If A can reach B, and B can reach C, then A can reach C
    # R(A, B) ^ R(B, C) => R(A, C)
    # !R(A, B) v !R(B, C) v R(A, C)
    # We can lower the amount of needed clauses by taking into account
    # that it is enough to impose this constraint on triples (A, B, C)
    # such that A and B are adjacent
    for v in vertices_by_coord:
        point_form = tuple([int(x) for x in v.split('-')])
        a = v
        adjv = ['-'.join(str(y) for y in x) \
        for x in get_neighbor_vertices(point_form, dim_sizes)]
        for b in adjv:
            for c in vertices_by_coord:
                if b == c: continue
                ret.add_clause(
                    (
                        (vname('R', a, b), False),
                        (vname('R', b, c), False),
                        (vname('R', a, c), True)
                    )
                )

    # No point must be self-reachable
    # This will prevent routings to have unnecesary cycles
    # This may be redundant if minimization is performed
    # for net in range( num_nets ):
    for var in vertices_by_coord:
        ret.add_clause(
            (
                (vname('R', var, var), False),
            )
        )

    return (parsed_input, ret)
