from formula import Formula

def parse_input(input_path):
    '''Parses (and checks) the file in the specified
    path. Returns a list of pairs of points.
    '''
    with open(input_path, 'r') as f:
        # Parse specification data
        values = map(int, f.readline().rstrip().split())
        num_dims = values[0]
        num_nets = values[1]
        dim_sizes = values[2:]

        assert len(dim_sizes) == num_dims,\
        'Number of specified dimensions don\'t match with sizes'
        assert min(dim_sizes) > 0,\
        'Dimensions cannot be null or negative'

        # Parse points coordinates
        points = []
        for line in f.readlines():
            current_points = map(int, line.rstrip().split())
            assert len(current_points) == 2 * num_dims, \
            'Point %s num dims don\'t match with file num dims'%current_points
            for i in range(2 * num_dims):
                assert current_points[i] > 0 and \
                current_points[i] <= dim_sizes[i % num_dims], \
                'Points coordinates must be inside the grid'
                current_points[i] -= 1
            s, t = current_points[num_dims:], current_points[:num_dims]
            points.append((tuple(s), tuple(t)))

        assert len(points) == num_nets,\
        'Input file contains a different number of nets than the one specified'

    ret = {
        'num_dims': num_dims,
        'dim_sizes': dim_sizes,
        'points': points
    }

    return ret

def convert_to_formula(input_path):
    '''Given an input file, returns a Formula object
    that represents the cell routing instance.
    '''
    # Parse the input, and check its integrity
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
    add_all_vertices(ret, [0] * num_dims, 0)
    # First constraint: we can have at most one of the nets passing
    # through some vertex. That is, if we have 2 nets, then only
    # one of 0-0-0 and 0-0-1 can be true at the same time. Note that
    # they can be all false
    # If the point is and endpoint, then it has exactly one valid
    # net. The rest of them can be directly set to False
    for vertex in vertices_by_coord:
        point_form = tuple([int(x) for x in vertex.split('-')])
        if point_form in endpoints:
            # Unit clauses that affirm or negate the appropriate
            # variables
            for k in range( num_nets ):
                ret.add_clause([(vertex + '-' + str(k), \
                k == netid[point_form])])
        else:
            # Conventional at most one
            to_add = []
            for k in range( num_nets ):
                to_add.append(vertex + '-' + str(k))
            ret.at_most(to_add, 1)

    # Second constraint: restrictions on type and amount of edges
    # More precisely, let's add constraints of the form
    # "a vertex has zero or two incident edges" for normal
    # vertices and "a vertex has exactly one incident edge" for
    # net endpoints
    def get_neighbor_vertices(position):
        # A point p is adjacent to other point q if we can get q
        # by adding or subtracting exactly one unit to some of its
        # coordinates
        from copy import deepcopy
        adj = list(deepcopy(position))
        ret = []
        for k in [-1, 1]:
            for i in range( num_dims ):
                adj[i] += k
                if adj[i] >= 0 and adj[i] < dim_sizes[i]:
                    ret.append(deepcopy(adj))
                adj[i] -= k
        return ret

    for vertex in vertices_by_coord:
        point_form = tuple([int(x) for x in vertex.split('-')])
        adj = get_neighbor_vertices(point_form)
        for net in range( num_nets ):
            adj_vertices = ['-'.join(str(x) for x in adjv) + '-' + str(net) \
            for adjv in adj]
            if point_form in endpoints:
                # Work only with corresponding nets here
                # (it makes no sense to do any other thing)
                if netid[point_form] != net:
                    continue
                ret.exactly(adj_vertices, 1)
            else:
                # For non-endpoint vertices, we can have zero or two
                # incident edges, but not one.
                # We can implement this as a combination of an
                # at-most-two and a not-exactly one constraint
                ret.at_most(adj_vertices, 2)
                ret.not_exactly(adj_vertices, 1)

    return (parsed_input, ret)

def get_used_vertices(formula, var_values):
    ret = []
    for i in range(1, len(var_values)):
        if var_values[i] > 0:
            ret.append(formula.dimacs2internal[abs(var_values[i])])
    return ret
