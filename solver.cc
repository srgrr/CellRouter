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

bool ok = true;

std::vector< int32_t > solve_formula(instance& ins, abstract_formula& form, std::vector< std::vector< int32_t > >& f,
  int var_count) {
  Minisat::Solver s;
  for(int i = 0; i < var_count; ++i) {
    s.newVar();
  }
  add_clauses(s, f);
  std::vector< int32_t > ret;
  auto edge_ids = form.get_edge_ids();
  bool ok = true;
  int bound = -1;

  using namespace PBLib;
  PBConfig config = std::make_shared< PBConfigClass >();
  VectorClauseDatabase g(config);
  PB2CNF pb2cnf(config);
  AuxVarManager auxvars(var_count + 1);

  std::vector< WeightedLit > literals;
  for(auto edge_id : edge_ids) {
    literals.push_back(WeightedLit(edge_id, 1));
  }
  IncPBConstraint constraint;
  bool first = true;

  while(ok) {
    std::cerr << "starting new iteration..." << std::endl;
    if(bound != -1) {
      int new_clauses;
      if(first) {
        constraint = IncPBConstraint(literals, LEQ, bound);
        pb2cnf.encodeIncInital(constraint, g, auxvars);
        first = false;
        new_clauses = int(g.getClauses().size());
      }
      else {
        int old_clauses = int(g.getClauses().size());
        constraint.encodeNewLeq(bound, g, auxvars);
        new_clauses = int(g.getClauses().size()) - old_clauses;
      }
      std::vector< std::vector< int32_t > > to_add;
      auto& constraint_clauses = g.getClauses();
      for(int i = 0; i < new_clauses; ++i) {
        to_add.push_back(constraint_clauses[constraint_clauses.size() - i - 1]);
        for(auto& x : to_add.back()) {
          var_count = std::max(var_count, x);
        }
      }
      while(s.nVars() < var_count) {
        s.newVar();
      }
      add_clauses(s, to_add);
    }
    std::clock_t t_begin = std::clock();

    std::future<bool> future = std::async(std::launch::async, [&s]() {
      return s.solve();
    });

    std::future_status status = future.wait_for(std::chrono::seconds(300));
    if(status == std::future_status::timeout) {
      if(!ret.empty()) {
        ret = form.unmark_extra_cycles(ins, ret);
        form.print_plottable(std::cout, ins, ret);
        std::exit(0);
      }
    } else if(status == std::future_status::ready) {
      ok = future.get();
    }
    std::cerr << "ok = " << std::boolalpha << ok << std::endl;
    if(ok) {
      ret.clear();
      for(int i = 0 ; i < int(s.model.size()); ++i) {
        ret.push_back(i + 1);
        if(s.model[i] == Minisat::lbool(uint8_t(1))) {
          ret.back() *= -1;
        }
      }
      bound = form.count_used_edges(ins, ret);
      std::cerr << bound;
      --bound;
    }
    std::clock_t t_end = std::clock();
    double elapsed_secs = double(t_end - t_begin) / CLOCKS_PER_SEC;
    std::cerr << " " << elapsed_secs << std::endl;
  }
  std::cerr << "deleting extra cycles..." << std::endl;
  return form.unmark_extra_cycles(ins, ret);
}
