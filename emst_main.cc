#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include "instance.h"
#include "emst.h"

void usage(int exit_code) {
    std::cout << "Usage: emst_main.exe <file-name>" << std::endl;
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
  instance ins = from_stream(ifs);
  // Only 2d is admitted here
  assert(ins.dim_sizes.size() == 2);
  compute_emst_subnets(ins);
  // Print a friendly format for drawing
  int num_nets = ins.nets.size();
  std::cout << num_nets << std::endl;
  for(int net = 0; net < num_nets; ++net) {
    int num_vertices = ins.nets[net].vertices.size();
    std::cout << num_vertices << std::endl;
    for(auto& vertex : ins.nets[net].vertices) {
      bool first = true;
      for(auto& component : vertex) {
        if(!first) {
          std::cout << " ";
        }
        first = false;
        std::cout << component;
      }
      std::cout << std::endl;
    }
    for(auto& uv : ins.nets[net].subnets) {
      std::cout << uv[0] << " " << uv[1] << std::endl;
    }
  }
}
