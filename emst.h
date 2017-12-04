#pragma once
#include <cmath>
#include <queue>
#include <random>
#include "instance.h"
const double EPS = 1e-9;

/*
  For each net, determine the subnets with the
  Minimum Euclidean Spanning Tree algorithm.
*/
void compute_emst_subnets(instance&);
