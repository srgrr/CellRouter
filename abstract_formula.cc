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

void abstract_formula::add_constraint(abstract_constraint::constraint* c) {
  constraints.push_back(c);
}

void abstract_formula::summary(std::ostream& os) {
  os << "Variables: " << variables.size() << std::endl;
  os << "Abstract constraints: " << constraints.size() << std::endl;
}

abstract_formula abstract_formula::from_instance(instance& ins) {
  abstract_formula ret;
  _add_variables(ins, ret);
  // First constraint (1): at most one net-subnet per edge
  std::vector< int > cur(ins.num_dims, 1);
  do {
    auto neighbs = edges_from_vertex(ins, cur);
    for(auto& edg : neighbs) {
      if(edg.u == cur) {
        std::vector< std::string > clause;
        for(int net = 0; net < ins.num_nets; ++net) {
          for(int subnet = 0; subnet < int(ins.nets[net].subnets.size()); ++subnet) {
            clause.push_back(variable(edg, net, subnet).get_name());
          }
        }
        abstract_constraint::constraint *amo = new abstract_constraint::at_most_k(1, clause);
        ret.add_constraint(amo);
      }
    }
  } while(_next(ins, cur));
  // Second constraint (1): Subnet endpoints have exactly one set edge
  std::set< std::vector< int > > processed_endpoints;
  for(int net = 0; net < ins.num_nets; ++net) {
    for(int subnet = 0; subnet < int(ins.nets[net].subnets.size()); ++subnet) {
      std::vector< std::vector< int > > endpoints = {
        ins.nets[net].vertices[ins.nets[net].subnets[subnet][0]],
        ins.nets[net].vertices[ins.nets[net].subnets[subnet][1]]
      };
      for(auto& endpoint : endpoints) {
        std::vector< int > var(endpoint);
        var.push_back(net);
        var.push_back(subnet);
        processed_endpoints.insert(var);
        auto neighbs = edges_from_vertex(ins, endpoint);
        std::vector< std::string > clause;
        for(auto& edg : neighbs) {
          clause.push_back(variable(edg, net, subnet).get_name());
        }
        abstract_constraint::constraint *exo = new abstract_constraint::exactly_k(1, clause);
        ret.add_constraint(exo);
      }
    }
  }
  // Second constraint (2): Subnet non-endpoints have either zero or two set
  // edges
  cur = std::vector< int >(ins.num_dims, 1);
  do {
    for(int net = 0; net < ins.num_nets; ++net) {
      for(int subnet = 0; subnet < int(ins.nets[net].subnets.size()); ++subnet) {
        std::vector< int > var(cur);
        var.push_back(net);
        var.push_back(subnet);
        if(processed_endpoints.count(var) == 0) {
          auto neighbs = edges_from_vertex(ins, cur);
          std::vector< std::string > clause;
          for(auto& edg : neighbs) {
            clause.push_back(variable(edg, net, subnet).get_name());
          }
          abstract_constraint::constraint *amt = new abstract_constraint::at_most_k(2, clause);
          abstract_constraint::constraint *neo = new abstract_constraint::not_exactly_one(clause);
          ret.add_constraint(amt);
          ret.add_constraint(neo);
        }
      }
    }
  } while(_next(ins, cur));
  return ret;
}

std::vector< std::vector< int32_t > > abstract_formula::sat_formula(int& first_free) {
  std::vector< std::vector< int32_t > > ret;
  first_free = int(variables.size()) + 1;
  for(auto constraint : constraints) {
    constraint->to_sat(ret, first_free, name2id);
  }
  --first_free;
  return ret;
}

void abstract_formula::print_plottable(std::ostream& os, instance& ins, std::vector< int32_t >& model) {
  os << ins.dim_sizes[0] << " " << ins.dim_sizes[1] << std::endl;
  for(int i = 0; i < int(variables.size()); ++i) {
    if(model[i] > 0) {
      std::cout << id2name.at(std::abs(model[i])) << std::endl;
    }
  }
}
