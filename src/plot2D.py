
def get_colors():
    import matplotlib.pyplot as plt
    return plt.rcParams['axes.prop_cycle'].by_key()['color']

def plot2D(used_vertices, data_info, output_file, uses_successors = False):
    from utils import vname
    import matplotlib.pyplot as plt
    used_vertices = set(used_vertices)
    plt.figure('Resulting routing')
    n, m = data_info['dim_sizes']
    plt.yticks(list(range(n + 2)))
    plt.xticks(list(range(m + 2)))
    colors = get_colors()

    for i in range(len(data_info['points'])):
        s, t = data_info['points'][i]
        plt.text(s[1] + 1 + 0.125, s[0] + 1 + 0.125, str(i+1))
        plt.text(t[1] + 1 + 0.125, t[0] + 1 + 0.125, str(i+1))
        plt.scatter(s[1] + 1, s[0] + 1, color = colors[i % len(colors)])
        plt.scatter(t[1] + 1, t[0] + 1, color = colors[i % len(colors)])

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
                        color = colors[int(net) % len(colors)], linewidth = 1)
        except:
            pass
    plt.savefig(output_file)
