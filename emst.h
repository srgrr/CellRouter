#pragma once
#include <cmath>
#include <queue>
#include "instance.h"

/*
  For each net, determine the subnets with the
  Minimum Euclidean Spanning Tree algorithm.
*/
void compute_emst_subnets(instance&);
