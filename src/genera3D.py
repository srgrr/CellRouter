import sys
import random

N = int(sys.argv[1])
Z = int(sys.argv[3])
M = int(sys.argv[2])

used_points = set()

print('3 %d %d %d %d'%(M, N, N, Z))
for i in range(M):
    x, y, z = -1, -1, -1
    while max(x, y, z) < 1 or (x, y, z) in used_points:
        x, y, z = random.randint(1, N), random.randint(1, N), random.randint(1, Z)
    t, v, w = -1, -1, -1
    while max(t, v, w) < 1 or (t, v, w) in used_points:
        t, v, w = random.randint(1, N), random.randint(1, N), random.randint(1, Z)
    print(x, y, z, t, v, w)
