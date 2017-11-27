
def get_colors():
    import matplotlib.pyplot as plt
    return plt.rcParams['axes.prop_cycle'].by_key()['color']

def plot3D(used_vertices, data_info, output_file):
    '''Prints n 2d plots
    '''
    for z in range( data_info['dim_sizes'][2] ):
        plot2D(used_vertices, z, data_info, \
        '%s-%d.png'%(output_file.replace('.png', ''), z + 1))

def plot2D(used_vertices, z, data_info, output_file, uses_successors = True):
    from utils import vname
    import matplotlib.pyplot as plt
    used_vertices = set(used_vertices)
    plt.figure('Resulting routing, level %d'%z)
    n, m, k = data_info['dim_sizes']
    plt.yticks(list(range(n + 2)))
    plt.xticks(list(range(m + 2)))
    colors = get_colors()

    for i in range(len(data_info['points'])):
        s, t = data_info['points'][i]
        if s[2] == z:
            plt.text(s[1] + 1 + 0.125, s[0] + 1 + 0.125, str(i+1))
            plt.scatter(s[1] + 1, s[0] + 1, color = colors[i % len(colors)])
        if t[2] == z:
            plt.scatter(t[1] + 1, t[0] + 1, color = colors[i % len(colors)])
            plt.text(t[1] + 1 + 0.125, t[0] + 1 + 0.125, str(i+1))

    for vertex in used_vertices:
        try:
            point_form = list(map(int, vertex.split('-')))[:-1]
            net = vertex.split('-')[-1]
            if point_form[2] != z: continue
            for k in [-1, 1]:
                for i in range(2):
                    from copy import deepcopy
                    adj = deepcopy(point_form)
                    adj[i] += k
                    adj_id = '-'.join(str(x) for x in adj) + '-' + net
                    if adj_id in used_vertices:
                        plt.plot([point_form[1] + 1, adj[1] + 1], [point_form[0] + 1, adj[0] + 1], \
                        color = colors[int(net) % len(colors)], linewidth = 1)
        except:
            import traceback
            traceback.print_exc()
    plt.savefig(output_file)
