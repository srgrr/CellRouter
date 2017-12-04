#pragma once
#include <iostream>
#include <vector>
#include "abstract_formula.h"
#include "abstract_constraint.h"
#include "minisat/core/Solver.h"

std::vector< int32_t > solve_formula(instance&, abstract_formula&, std::vector< std::vector< int32_t > >&, int);
