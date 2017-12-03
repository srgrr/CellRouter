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

  int at_most_k::get_k() {
    return k;
  }

  at_most_k::at_most_k(int k, std::vector< std::string >& vars) :
  constraint(vars) {
    this->k = k;
  }


  void at_most_k::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int >& name2id) {
    std::cout << "to_sat at_most_k" << std::endl;
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

  void exactly_k::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int >& name2id) {
    std::cout << "to_sat exactly_k" << std::endl;
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

  void not_exactly_one::to_sat(std::vector< std::vector< int32_t > >& formula,
  int& first_free, std::map< std::string, int >& name2id) {
    std::cout << "to_sat not_exactly_one" << std::endl;
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
