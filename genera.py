import sys

N, M = map(int, sys.argv[1:])

def main():
  print('2 %d %d %d'%(M, N, N))
  from random import randint
  for i in range(M):
    k = 2
    print(k)
    for j in range(k):
      print('%d %d'%(randint(1, N), randint(1, N)))



if __name__ == "__main__":
  main()
