#include "instance.h"

/*
  Generate a random point inside a grid
*/
std::vector< int > random_point(std::vector< int >& bounds) {
  int n = int(bounds.size());
  std::vector< int > ret(n);
  for(int i=0; i < n; ++i) {
    ret[i] = random_integer(1, bounds[i]);
  }
  return ret;
}

/*
  Given two points, and a net, add all the possible Ls to the set of allowed
  paths.
*/
void add_ls(instance& ins,
            const std::vector<int>& s,
            const std::vector<int>& t,
            int net) {
  std::vector< int > perm(ins.num_dims);
  for(int i = 0; i < ins.num_dims; ++i) {
    perm[i] = i;
  }
  do {
    std::vector< int > cur = s;
    for(int i = 0; i < ins.num_dims; ++i) {
      int d = perm[i];
      int plus = sgn(t[d] - cur[d]);
      while(cur[d] != t[d]) {
        ins.allowed_variables.insert(variable(cur, net));
        cur[d] += plus;
      }
      ins.allowed_variables.insert(variable(cur, net));
    }
  } while(next_permutation(perm.begin(), perm.end()));
}

/*
  Apply the R-L "Random-L" algorithm to each net in order to generate
  some paths.
*/
void generate_random_l_paths(instance& ins, int k = 5) {
  for(int net = 0; net < ins.num_nets; ++net) {
    // For each pair (s, t), let's "activate" the following paths:
    // The d! "Ls" between s and t
    // K random paths s -> k -> t (d! "Ls" s->k, d! "Ls" k->t)

    // Add the two main set of "Ls"
    add_ls(ins, ins.points[net][0], ins.points[net][1], net);

    // Generate k random midpoints
    // and add (s, k) -> (k, t)
    for(int i = 0; i < k; ++i) {
      std::vector< int > midpoint = random_point(ins.dim_sizes);
      add_ls(ins, ins.points[net][0], midpoint, net);
      add_ls(ins, midpoint, ins.points[net][1], net);
    }
  }
}

instance parse_input(std::ifstream& in) {
  instance ret = instance();
  // Read the data from the input stream
  in >> ret.num_dims >> ret.num_nets;
  ret.dim_sizes = std::vector< int >(ret.num_dims);
  for(int& dim : ret.dim_sizes) {
    in >> dim;
  }
  for(int i = 0; i < ret.num_nets; ++i) {
    std::vector< std::vector< int > >
      source_sink(2, std::vector<int>(ret.num_dims));
    for(auto& point : source_sink) {
      for(auto& x : point) {
        in >> x;
      }
    }
    ret.points.push_back(source_sink);
  }
  // Apply the R-L algorithm to the input instance in order to enable
  // some random paths
  generate_random_l_paths(ret);
  return ret;
}
