#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include "instance.h"
#include "emst.h"
#include "abstract_formula.h"
#include "solver.h"

void usage(int exit_code) {
    std::cout << "Usage: router.exe <file-name>" << std::endl;
    std::exit(exit_code);
}

int main(int argc, char **argv) {
  if(argc != 2) {
    usage(1);
  }
  std::string file_name(argv[1]);
  std::ifstream ifs(file_name);
  if(!ifs.good()) {
    std::cout << "Cannot open file " << file_name << std::endl;
    usage(2);
  }
  auto ins = from_stream(ifs);
  compute_emst_subnets(ins);
  auto f = abstract_formula::from_instance(ins);
  int var_count = -1;
  auto sf = f.sat_formula(var_count);
  auto sol = solve_formula(ins, f, sf, var_count);
  if(!sol.empty()) {
    f.print_plottable(std::cout, ins, sol);
  }
  else {
    std::cout << "UNSAT" << std::endl;
  }

}
