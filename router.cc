#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include "instance.h"
#include "emst.h"

void usage(int exit_code) {
    std::cout << "Usage: router.exe <file-name>" << std::endl;
    std::exit(exit_code);
}

instance parse_input(std::ifstream& in) {
  instance ret;
  int num_dims, num_nets;
  assert(!in.eof());
  in >> num_dims >> num_nets;
  ret.dim_sizes = std::vector< int >(num_dims);
  ret.nets      = std::vector< net >(num_nets);
  for(int& x : ret.dim_sizes) {
    assert(!in.eof());
    in >> x;
  }
  for(int net = 0; net < num_nets; ++net) {
    int num_vertices;
    assert(!in.eof());
    in >> num_vertices;
    ret.nets[net].vertices = std::vector< std::vector< int > >(num_vertices,
    std::vector< int >(num_dims));
    for(auto& vertex : ret.nets[net].vertices) {
      for(auto& component : vertex) {
        assert(!in.eof());
        in >> component;
      }
    }
  }
  return ret;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    usage(1);
  }
  std::string file_name(argv[1]);
  std::ifstream ifs(file_name);
  if(!ifs.good()) {
    std::cout << "Cannot open file " << file_name << std::endl;
    usage(1);
  }
  instance ins = parse_input(ifs);
  compute_emst_subnets(ins);
}
