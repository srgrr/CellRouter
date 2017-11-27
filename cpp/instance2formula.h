#pragma once
#include "instance.h"
#include "types.h"
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <pblib/pb2cnf.h>
#include <pblib/weightedlit.h>
#include <pblib/incpbconstraint.h>
#include <pblib/VectorClauseDatabase.h>
using namespace PBLib;

/*
  Function that given an instance, generates the basic constraints from it.
  Basic constraints are:
  1) For each vertex, at most one net passes through it.
  2) Each endpoint has exactly one neighbor that "contains" the same net.
  3) Each non-endpoint is either not set or it has exactly two set neighbors (from the same net).
*/
std::vector< std::vector< literal > > generate_basic_formula(const instance& ins);

/*
  Function that given an instance and its basic formula,
  performs an iterative optimization to it.
*/
// #error "OPTIMIZE_BASIC_FORMULA MUST BE DEFINED"


/*
  Function that wraps the two previous functions.
  That is, given an instance, performs the whole optimization process.
*/
// #error "FUNCTION BASIC_OPT_WRAPPER MUST BE DEFINED"
