
def parse_input(input_path):
    '''Parses (and checks) the file in the specified
    path. Returns a list of pairs of points.
    '''
    with open(input_path, 'r') as f:
        # Parse specification data
        values = list(map(int, f.readline().rstrip().split()))
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
            current_points = list(map(int, line.rstrip().split()))
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
        'num_nets': num_nets,
        'dim_sizes': dim_sizes,
        'points': points
    }

    return ret
