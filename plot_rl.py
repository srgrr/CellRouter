


def main():
  import sys
  file_name = sys.argv[1]
  import subprocess
  output = subprocess.check_output(['./plot_rl_algo', file_name]).decode('utf-8')
  lines = [x.strip() for x in output.split('\n')]
  n, m = map(int, lines[0].split())
  import matplotlib.pyplot as plt
  plt.figure('Usable region plot')
  plt.xlim([0, m+1])
  plt.ylim([0, n+1])
  for line in lines[2:len(lines) - 1]:
    x, y = map(int, line.split())
    plt.scatter(y, x, color = 'red')
  x1, y1, x2, y2 = map(int, lines[1].split())
  plt.scatter(y1, x1, color = 'black')
  plt.scatter(y2, x2, color = 'black')
  plt.show()


if __name__ == "__main__":
  main()
