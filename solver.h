#pragma once
#include <future>
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <ctime>
#include "abstract_formula.h"
#include "abstract_constraint.h"
#include <pblib/pb2cnf.h>
#include <minisat/core/Solver.h>

std::vector< int32_t > solve_formula(instance&, abstract_formula&, std::vector< std::vector< int32_t > >&, int);
