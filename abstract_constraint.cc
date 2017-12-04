#include "abstract_constraint.h"

namespace abstract_constraint {

  constraint::constraint() {

  }

  constraint::constraint(std::vector< int32_t >& vars) {
    variables = vars;
  }

  void constraint::add_variable(int32_t var_name) {
    variables.push_back(var_name);
  }

  const std::vector< int32_t >& constraint::get_variables() {
    return variables;
  }

  void constraint::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free) {
    std::cout << "THIS SHOULD NOT EVER NEVER EVAH APPEAR" << std::endl;
  }

  int at_most_k::get_k() {
    return k;
  }

  at_most_k::at_most_k(int k, std::vector< int32_t >& vars) :
  constraint(vars) {
    this->k = k;
  }


  void at_most_k::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free) {
    PB2CNF pb2cnf;
    first_free = pb2cnf.encodeAtMostK(variables, k, formula, first_free) + 1;
  }

  int exactly_k::get_k() {
    return k;
  }

  exactly_k::exactly_k(int k, std::vector< int32_t >& vars) :
  constraint(vars) {
    this->k = k;
  }

  void exactly_k::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free) {
    PB2CNF pb2cnf;
    first_free = pb2cnf.encodeAtMostK(variables, k, formula, first_free) + 1;
    first_free = pb2cnf.encodeAtLeastK(variables, k, formula, first_free) + 1;
  }

  not_exactly_one::not_exactly_one(std::vector< int32_t >& vars) :
  constraint(vars) {
  }

  void not_exactly_one::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free) {
    auto lits = variables;
    for(auto& x : lits) {
      x = -x;
      formula.push_back(lits);
      x = -x;
    }
  }

  cnfclause::cnfclause(std::vector< int32_t >& vars) :
  constraint(vars) {
  }

  void cnfclause::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free) {
    formula.push_back(variables);
  }

};
