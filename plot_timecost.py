import sys

filename = sys.argv[1]

import matplotlib.pyplot as plt

data = [x.strip().split(' ') for x in open(filename).readlines()[:-1]]

print(data)

x, y = [], []

total = 0.0

for (cost, time) in data:
  cost = float(cost)
  time = float(time)
  x.append(time + total)
  y.append(cost)
  total += time

plt.figure('Time vs cost')
plt.xlabel('Ellapsed time')
plt.ylabel('Solution cost')

plt.plot(x, y)
plt.scatter(x, y)

plt.show()
