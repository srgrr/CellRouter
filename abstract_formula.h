#pragma once
#include <iostream>
#include <cstring>
#include <set>
#include <map>
#include "instance.h"

class abstract_formula {
private:
  std::set< std::string > variables;
  std::map< int, std::string > id2name;
  std::map< std::string, int > name2id;
public:
  abstract_formula() {}
  void add_variable(std::string var_name);

  static abstract_formula from_instance(instance&);

};
