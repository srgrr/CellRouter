#include "instance.h"

instance::instance() {
  dim_sizes = std::vector< int >();
  nets = std::vector< net >();
}

instance from_stream(std::ifstream& in) {
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
