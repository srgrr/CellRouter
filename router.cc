#include <pblib/pb2cnf.h>
#include "types.h"
#include "instance.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

void usage(int exit_code) {
  std::cout << "Usage: router <path-to-file>" << std::endl;
  std::exit(exit_code);
}

int main(int argc, char** argv) {
  if( argc != 2 ) {
    usage(1);
  }
  std::string input_file(argv[1]);
  std::ifstream in(input_file);
  if(in.bad()) {
    std::cout << "ERROR: Could not find file " << input_file << std::endl;
    usage(1);
  }
  auto initial_instance = parse_input(in);
  initial_instance.summary(std::cerr);
}
