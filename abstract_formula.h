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
  std::set< std::string > aux_variables;
  std::vector< int32_t > edge_ids;
  std::map< int, std::string > id2name;
  std::map< std::string, int > name2id;
  std::vector< abstract_constraint::constraint* > constraints;
  int var_count = 1;
  bool add_to(std::string, std::set< std::string >&);
public:
  abstract_formula() {}
  bool add_variable(std::string var_name);
  bool add_aux_variable(std::string var_name);
  void add_constraint(abstract_constraint::constraint*);
  void summary(std::ostream&);
  void add_edge_var(int32_t);
  static abstract_formula from_instance(instance&);
  std::vector< std::vector< int32_t > > sat_formula(int&);
  void print_plottable(std::ostream&, instance&, std::vector< int32_t >&);
  const std::map< std::string, int >& get_name2id();
  int count_used_edges(instance&, std::vector< int32_t >&);
  int get_var_count();
  std::vector< int32_t > get_edge_ids();
};
