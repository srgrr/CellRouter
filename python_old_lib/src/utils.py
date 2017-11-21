def vname(*args):
    '''Join various strings in a single string S1-S2...-SN
    '''
    return '-'.join(args)

def point2var(p):
    '''Given a list of integers, joins them in a string, separated by
    dashes
    '''
    return '-'.join(str(x) for x in p)

def get_neighbor_vertices(position, dim_sizes):
    '''Given a vertex, and the limits of its dimensions,
    return a list with all of its valid neighbors.
    A point p is adjacent to other point q if we can get q
    by adding or subtracting exactly one unit to some of its
    coordinates
    '''
    from copy import deepcopy
    adj = list(deepcopy(position))
    ret = []
    for k in [-1, 1]:
        for i in range( len(position) ):
            adj[i] += k
            if adj[i] >= 0 and adj[i] < dim_sizes[i]:
                ret.append(deepcopy(adj))
            adj[i] -= k
    return ret


def clear_cycles(inp, used_variables):
    '''Given an input, and a solution, remove from set decisional variables
    the ones that form unwanted cycles.
    Necessary for some encodings, as not all of them prevent cycles to appear.
    '''
    ret = set()
    for net in range( inp['num_nets'] ):
        snet = str(net)
        source, sink = inp['points'][net]
        ret.add(vname(point2var(source), snet))
        ret.add(vname(point2var(sink),   snet))
        cur_point = source
        previous = None
        while cur_point != sink:
            neighbs = [tuple(x) for x in get_neighbor_vertices(cur_point, inp['dim_sizes'])]
            neighb = None
            for n in neighbs:
                if n != previous and vname(point2var(n), snet) in used_variables:
                    neighb = n
                    break
            assert neighb is not None, 'Some source or intermediate vertex had no next!'
            ret.add(vname(point2var(neighb), snet))
            previous = cur_point
            cur_point = neighb

    # Add non-vertex variables, too
    for var in used_variables:
        if not var.split('-')[0].isdigit():
            ret.add(var)
    return ret
