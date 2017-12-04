import matplotlib.pyplot as plt
import sys
import subprocess

def get_color(x):
    l = plt.rcParams['axes.prop_cycle'].by_key()['color']
    return l[x % len(l)]

def main():
    lines = subprocess.check_output(['./router.exe', sys.argv[1]]).decode('utf-8').split('\n')
    n, m = map(int, lines[0].split(' '))
    plt.figure('Result')
    endpoints = False
    for line in lines[1:]:
        print(line)
        try:
            if not endpoints:
                x1, y1, x2, y2, net, subnet = map(int, line.split('-'))
                plt.plot([x1, x2], [y1, y2], color = get_color(net))
                plt.scatter(x1, y1, color = get_color(net), s = 8)
                plt.scatter(x2, y2, color = get_color(net), s = 8)
            else:
                x, y, net = map(int, line.split())
                plt.scatter(x, y, color = get_color(net), s = 24)
        except:
            import traceback
            traceback.print_exc()
            endpoints = True
    plt.show()

if __name__ == '__main__':
    main()
