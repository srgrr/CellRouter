#pragma once
#include <iostream>
#include <cstring>
#include <set>
#include <map>
#include "instance.h"
#include "abstract_constraint.h"

class abstract_formula {
private:
  std::set< std::string > variables;
  std::map< int, std::string > id2name;
  std::map< std::string, int > name2id;
  std::vector< abstract_constraint::constraint* > constraints;
public:
  abstract_formula() {}
  void add_variable(std::string var_name);
  void add_constraint(abstract_constraint::constraint*);
  void summary(std::ostream&);
  static abstract_formula from_instance(instance&);
  std::vector< std::vector< int32_t > > sat_formula();

};
