#include <pblib/pb2cnf.h>
#include <iostream>

int main() {
  PB2CNF pb2cnf;
  vector< int64_t > weights = {3,-2,7};
  vector< int32_t > literals = {-1,-2,3};
  vector< vector< int32_t > > formula;
  int32_t firstFreshVariable = 4;
  firstFreshVariable = pb2cnf.encodeGeq(weights, literals, -4, formula, firstFreshVariable) + 1; 
 
}
