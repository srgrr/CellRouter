
N = 10
M = 3

def main():
  print('2 %d %d %d'%(M, N, N))
  from random import randint
  for i in range(M):
    k = randint(2, 4)
    print(k)
    for _ in range(k):
      x, y = randint(1, N), randint(1, N)
      print('%d %d'%(x, y))



if __name__ == "__main__":
  main()
