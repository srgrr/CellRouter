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

  int at_most_k::get_k() {
    return k;
  }

  at_most_k::at_most_k(int k, std::vector< std::string >& vars) :
  constraint(vars) {
    this->k = k;
  }

  std::ostream& operator<<(std::ostream& os, at_most_k& amk) {
    os << "AM-"+std::to_string(amk.get_k())+"(";
    bool first = true;
    for(auto& var : amk.get_variables()) {
      if(!first) {
        os << ", ";
      }
      first = false;
      os << var;
    }
    os << ")";
    return os;
  }

  int exactly_k::get_k() {
    return k;
  }

  exactly_k::exactly_k(int k, std::vector< std::string >& vars) :
  constraint(vars) {
    this->k = k;
  }

  std::ostream& operator<<(std::ostream& os, exactly_k& exk) {
    os << "EX-"+std::to_string(exk.get_k())+"(";
    bool first = true;
    for(auto& var : exk.get_variables()) {
      if(!first) {
        os << ", ";
      }
      first = false;
      os << var;
    }
    os << ")";
    return os;
  }

  not_exactly_one::not_exactly_one(std::vector< std::string >& vars) :
  constraint(vars) {
  }

  std::ostream& operator<<(std::ostream& os, not_exactly_one& neo) {
    os << "NEO(";
    bool first = true;
    for(auto& var : neo.get_variables()) {
      if(!first) {
        os << ", ";
      }
      first = false;
      os << var;
    }
    os << ")";
    return os;
  }

};
