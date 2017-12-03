#pragma once
#include <iostream>
#include <vector>
#include "minisat/core/Solver.h"

std::vector< int32_t > solve_formula(std::vector< std::vector< int32_t > >& f, int);
