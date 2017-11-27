#include <pblib/pb2cnf.h>
#include <iostream>
#include <vector>

int main() {
  PB2CNF pb2cnf;
  std::vector< int64_t > weights = {3,-2,7};
  std::vector< int32_t > literals = {-1,-2,3};
  std::vector< std::vector< int32_t > > formula;
  int32_t firstFreshVariable = 4;
  firstFreshVariable = pb2cnf.encodeGeq(weights, literals, -4, formula, firstFreshVariable) + 1; 
 
}
