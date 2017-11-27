import sys
import random

N = int(sys.argv[1])
M = int(sys.argv[2])

used = []

for _ in range(N + 1):
    used.append([False] * (N + 1))

print('2 %d %d %d'%(M, N, N))

for _ in range(M):
    i, j = random.randint(1, N), random.randint(1, N)
    while used[i][j]:
        i, j = random.randint(1, N), random.randint(1, N)
    used[i][j] = True
    attempts = random.randint(1, N * N)
    oi, oj = i, j
    for _ in range(attempts):
        adji = oi + random.choice([-1, 1])
        adjj = oj + random.choice([-1, 1])
        if adji < 1 or adji > N or adjj < 1 or adjj > N or used[adji][adjj]:
            continue
        used[adji][adjj] = True
        oi, oj = adji, adjj
    print('%d %d %d %d'%(i, j, oi, oj))
