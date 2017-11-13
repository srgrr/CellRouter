
def get_colors():
    import pylab as plt
    return plt.rcParams['axes.prop_cycle'].by_key()['color']

def plot2D(used_vertices, data_info, output_file, uses_successors = True):
    from utils import vname
    import pylab as plt
    used_vertices = set(used_vertices)
    plt.figure('Resulting routing')
    n, m = data_info['dim_sizes']
    plt.ylim([0, n + 1])
    plt.xlim([0, m + 1])
    colors = get_colors()
    for vertex in used_vertices:
        try:
            point_form = list(map(int, vertex.split('-')))[:-1]
            net = vertex.split('-')[-1]
            for k in [-1, 1]:
                for i in range(2):
                    from copy import deepcopy
                    adj = deepcopy(point_form)
                    adj[i] += k
                    adj_id = '-'.join(str(x) for x in adj) + '-' + net
                    if adj_id in used_vertices and \
                    (not uses_successors or \
                    vname('S', '-'.join(str(x) for x in point_form), '-'.join(str(x) for x in adj)) in used_vertices):
                        plt.plot([point_form[1] + 1, adj[1] + 1], [point_form[0] + 1, adj[0] + 1], \
                        color = colors[int(net) % len(colors)], linewidth = 2)
                    plt.scatter(point_form[1] + 1, point_form[0] + 1, color = colors[int(net) % len(colors)])
        except:
            pass
    plt.savefig(output_file)
