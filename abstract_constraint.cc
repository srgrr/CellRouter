#include "abstract_constraint.h"

namespace abstract_constraint {

  constraint::constraint() {

  }

  constraint::constraint(std::vector< std::string >& vars) {
    variables = vars;
  }

  void constraint::add_variable(std::string& var_name) {
    variables.push_back(var_name);
  }

  const std::vector< std::string >& constraint::get_variables() {
    return variables;
  }

  void constraint::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int>& name2id) {
    std::cout << "THIS SHOULD NOT EVER NEVER EVAH APPEAR" << std::endl;
  }

  std::vector< int32_t > constraint::get_literals(std::map< std::string, int>& name2id) {
    std::vector< int32_t > ret;
    for(std::string& var : variables) {
      ret.push_back(name2id.at(var));
    }
    return ret;
  }

  int at_most_k::get_k() {
    return k;
  }

  at_most_k::at_most_k(int k, std::vector< std::string >& vars) :
  constraint(vars) {
    this->k = k;
  }


  void at_most_k::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int >& name2id) {
    PB2CNF pb2cnf;
    first_free = pb2cnf.encodeAtMostK(get_literals(name2id), k, formula, first_free) + 1;
  }

  int exactly_k::get_k() {
    return k;
  }

  exactly_k::exactly_k(int k, std::vector< std::string >& vars) :
  constraint(vars) {
    this->k = k;
  }

  void exactly_k::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int >& name2id) {
    PB2CNF pb2cnf;
    first_free = pb2cnf.encodeAtMostK(get_literals(name2id), k, formula, first_free) + 1;
    first_free = pb2cnf.encodeAtLeastK(get_literals(name2id), k, formula, first_free) + 1;
  }

  not_exactly_one::not_exactly_one(std::vector< std::string >& vars) :
  constraint(vars) {
  }

  void not_exactly_one::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int >& name2id) {
    auto lits = get_literals(name2id);
    for(auto& x : lits) {
      x = -x;
    }
    for(auto& x : lits) {
      x = -x;
      formula.push_back(lits);
      x = -x;
    }
  }

  implication::implication(std::vector< std::string >& impl, std::vector< std::string >& vars) :
  constraint(vars) {
    implicant = impl;
  }

  void implication::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int >& name2id) {
    std::vector< int32_t > impl;
    for(auto& lit : implicant) {
      impl.push_back(name2id.at(lit));
    }
    for(auto lit : get_literals(name2id)) {
      impl.push_back(lit);
    }
    formula.push_back(impl);
  }

};
