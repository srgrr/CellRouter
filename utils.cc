#include "utils.h"

std::mt19937& mtgen() {
  static std::random_device rd;
  static std::mt19937 rng(rd());
  return rng;
}

int random_integer(int l, int h) {
  std::uniform_int_distribution<> dis(l, h);
  return dis(mtgen());
}


std::vector< int > random_permutation(int n) {
  std::vector< int > ret(n);
  for(int i = 0; i < n; ++i) {
    ret[i] = i + 1;
  }
  for(int i = 1; i < n; ++i) {
    int j = random_integer(0, i - 1);
    std::swap(ret[i], ret[j]);
  }
  return ret;
}

int sgn(int n) {
  return n >= 0 ? 1 : -1;
}
