#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>
#include <map>
#include "utils.h"
#include <random>

/*
  A variable is a struct that contains the following information:
  - Coordinates
  - Net
*/
struct variable {
  std::vector< int > coords;
  int net;

  variable();
  variable(std::vector< int > c, int n);
  friend bool operator<(const variable& a, const variable& b);
  friend std::ostream& operator<<(std::ostream&, const variable& v);
};

/*
  An instance is a struct that contains the following information:
  - Number of dimensions
  - Number of nets
  - Dimension boundaries for each point
  - Source-Sink pairs
  - A set of valid point-net pairs (randomly generated by the R-L algo)
*/
struct instance {
  int num_dims, num_nets;
  std::vector< int > dim_sizes;
  std::vector< std::vector< std::vector< int > > > points;
  std::set< variable > allowed_variables;
  std::map< variable, int > var2index;
  std::map< int, variable > index2var;

  instance();

  void summary(std::ostream& out);

};

/*
  Parses an input file, creates an instance,
  applies the R-L algorithm to it, and returns
  the resulting instance
*/
instance parse_input(std::ifstream&);
