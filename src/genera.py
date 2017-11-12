import sys
import random

N = int(sys.argv[1])
M = int(sys.argv[2])

used_points = set()

print('2 %d %d %d'%(M, N, N))
for i in range(M):
    x, y = -1, -1
    while max(x, y) < 1 or (x, y) in used_points:
        x, y = random.randint(1, N), random.randint(1, N)
    z, t = -1, -1
    while max(z, t) < 1 or (z, t) in used_points:
        z, t = random.randint(1, N), random.randint(1, N)
    print(x, y, z, t)
