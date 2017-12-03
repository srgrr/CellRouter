#include "abstract_formula.h"

struct edge {
  std::vector< int > u;
  std::vector< int > v;
  edge() {
    u = std::vector< int >();
    v = std::vector< int >();
  }
  edge(std::vector< int >& a, std::vector< int >& b) {
    u = a;
    v = b;
    if(u > v) {
      std::swap(u, v);
    }
  }
  friend bool operator<(const edge& a, const edge& b) {
    if(a.u != b.u) {
      return a.u < b.u;
    }
    return a.v < b.v;
  }
};

struct variable {
  edge e;
  int net, subnet;
  variable(edge edg, int netid, int subnetid) {
    e = edg;
    net = netid;
    subnet = subnetid;
  }
  std::string get_name() {
    std::string ret;
    for(int i = 0; i < int(e.u.size()); ++i) {
      ret += std::to_string(e.u[i]) + "-";
    }
    for(int i = 0; i < int(e.v.size()); ++i) {
      ret += std::to_string(e.v[i]) + "-";
    }
    ret += std::to_string(net) + "-" + std::to_string(subnet);
    return ret;
  }
};

bool _next(instance& ins, std::vector< int >& cur) {
  int n = int(cur.size()), carry = 1;
  for(int i = 0; i < n; ++i) {
    cur[i] += carry;
    carry = cur[i] > ins.dim_sizes[i] ? 1 : 0;
    cur[i] = (cur[i] - 1) % ins.dim_sizes[i] + 1;
    if(carry == 0) return true;
  }
  return false;
}

std::vector< edge > edges_from_vertex(instance& ins, std::vector< int >& cur) {
  std::vector< edge > ret;
  std::vector< int > v = cur;
  for(int i = 0; i < int(cur.size()); ++i) {
    for(int j = -1; j <= 1; j += 2) {
      v[i] += j;
      if(v[i] >= 1 && v[i] <= ins.dim_sizes[i]) {
        edge edg(cur, v);
        ret.push_back(edg);
      }
      v[i] -= j;
    }
  }
  return ret;
}

void _add_variables(instance& ins, abstract_formula& ret) {
  std::vector< int > cur(ins.num_dims, 1);
  do {
    for(auto edg : edges_from_vertex(ins, cur)) {
      for(int i = 0; i < ins.num_nets; ++i) {
        for(int j = 0; j < int(ins.nets[i].subnets.size()); ++j) {
          ret.add_variable(variable(edg, i, j).get_name());
        }
      }
    }
  } while(_next(ins, cur));
}

void abstract_formula::add_variable(std::string var_name) {
  if(variables.count(var_name) == 0) {
    variables.insert(var_name);
    int var_id = int(variables.size());
    id2name[var_id] = var_name;
    name2id[var_name] = var_id;
  }
}

abstract_formula abstract_formula::from_instance(instance& ins) {
  abstract_formula ret;
  _add_variables(ins, ret);
  return ret;
}
