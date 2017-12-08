#include "solver.h"

void add_clauses(Minisat::Solver& s, std::vector< std::vector< int32_t > >& f) {
  for(auto& clause : f) {
    Minisat::vec< Minisat::Lit > v;
    for(auto x : clause) {
      auto ax = std::abs(x);
      if(x < 0) {
        v.push(~Minisat::mkLit(ax - 1));
      }
      else {
        v.push(Minisat::mkLit(ax - 1));
      }
    }
    s.addClause(v);
  }
}

std::vector< int32_t > solve_formula(instance& ins, abstract_formula& form, std::vector< std::vector< int32_t > >& f,
  int var_count) {
  Minisat::Solver s;
  for(int i = 0; i < var_count; ++i) {
    s.newVar();
  }
  add_clauses(s, f);
  std::vector< int32_t > ret;
  auto edge_ids = form.get_edge_ids();
  if(s.solve()) {
    // We have a basic solution, let's try to improve it
    for(int i = 0 ; i < int(s.model.size()); ++i) {
      ret.push_back(i + 1);
      if(s.model[i] == Minisat::lbool(uint8_t(1))) {
        ret.back() *= -1;
      }
    }
    int cost = form.count_used_edges(ins, ret);
    std::cerr << "basic solution found (cost " << cost << ")" << std::endl;
    bool ok = true;
    int bound = cost - 1;
    while(ok && bound > 295) {
      std::vector< std::vector< int32_t > > g;
      abstract_constraint::at_most_k amk(bound, edge_ids);
      amk.to_sat(g, var_count);
      while(s.nVars() < var_count) {
        s.newVar();
      }
      add_clauses(s, g);
      if((ok = s.solve())) {
        s.simplify();
        ret.clear();
        for(int i = 0 ; i < int(s.model.size()); ++i) {
          ret.push_back(i + 1);
          if(s.model[i] == Minisat::lbool(uint8_t(1))) {
            ret.back() *= -1;
          }
        }
        bound = form.count_used_edges(ins, ret);
        std::cerr << "Found cost " << bound << std::endl;
        --bound;
      }
      else {
        std::cerr << "Failed to find bound " << bound << std::endl;
      }
    }
  }
  return ret;
}
