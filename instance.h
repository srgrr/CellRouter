#pragma once
#include <vector>
#include "net.h"

struct instance {
  int num_dims, num_nets;
  std::vector< int > dim_sizes;
  std::vector< net > nets;
  instance();
};
