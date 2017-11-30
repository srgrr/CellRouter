#pragma once
#include <vector>

struct net {
  std::vector< std::vector< int > > vertices;
  std::vector< std::vector< int > > subnets;
  net();
};
