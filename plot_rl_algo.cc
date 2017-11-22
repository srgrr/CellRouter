#include <iostream>
#include <fstream>
#include "instance.h"

/*
  Used program to extract "activation figures".
  Intended to be used in 2D instances.
  Reads an instance, applies the RL algorithm to it,
  and outputs the used variables, one per line, of
  the first net.
  This can be plotted in order to achieve a better
  understanding of the R-L algorithm, and how
  "non-intended" paths appear.
*/
int main(int argc, char** argv) {
  std::string file_name(argv[1]);
  std::ifstream in(file_name);
  instance ins = parse_input(in);
  // Summary is printed in a different stream
  ins.summary(std::cerr);
  std::cout << ins.dim_sizes[0] << " " << ins.dim_sizes[1] << std::endl;
  for(int k = 0; k < 2; ++k) {
    for(int i = 0; i < 2; ++i) {
      if(i > 0) {
        std::cout << " ";
      }
      std::cout << ins.points[0][k][i];
    }
    std::cout << " ";
  }
  std::cout << std::endl;
  for(auto& var : ins.allowed_variables) {
    if(var.net != 0) continue;
    bool first = true;
    for(auto x : var.coords) {
      if(!first) {
        std::cout << " ";
      }
      first = false;
      std::cout << x << " ";
    }
    std::cout << std::endl;
  }
}
