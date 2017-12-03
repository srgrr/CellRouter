#include "solver.h"

std::vector< int32_t > solve_formula(std::vector< std::vector< int32_t > >& f,
  int var_count) {
  using namespace Minisat;
  Solver s;
  for(int i = 0; i < var_count; ++i) {
    s.newVar();
  }
  for(auto& clause : f) {
    vec< Lit > v;
    for(auto x : clause) {
      auto ax = std::abs(x);
      if(x < 0) {
        v.push(~mkLit(ax - 1));
      }
      else {
        v.push( mkLit(ax - 1));
      }
    }
    s.addClause(v);
  }
  std::vector< int32_t > ret;
  if(s.solve()) {
    for(int i = 0 ; i < s.model.size(); ++i) {
      ret.push_back(i + 1);
      if(s.model[i] == l_False) {
        ret.back() *= -1;
      }
    }
  }
  return ret;
}
