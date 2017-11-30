import matplotlib.pyplot as plt
import sys
import subprocess

def get_color(x):
    l = plt.rcParams['axes.prop_cycle'].by_key()['color']
    return l[x % len(l)]

def main():
    file_name = sys.argv[1]
    output = subprocess.check_output(
    ['./emst_main.exe', file_name]
    ).decode('utf-8')
    lines = output.split('\n')
    num_nets = int(lines[0])
    cur = 1
    plt.figure('EMST')
    for net in range( num_nets ):
        num_vertices = int(lines[cur])
        cur += 1
        vertices = []
        for i in range( num_vertices ):
            vertices.append(
                list(map(int, lines[cur + i].split()))
            )
            plt.scatter(*vertices[-1], color = get_color(net))
        cur += num_vertices
        for i in range( num_vertices - 1):
            line = list(map(int, lines[cur + i].split()))
            plt.plot([vertices[line[0]][0], vertices[line[1]][0]],
            [vertices[line[0]][1], vertices[line[1]][1]],
            color = get_color(net)
            )

        cur += num_vertices - 1

    plt.show()

if __name__ == '__main__':
    main()
