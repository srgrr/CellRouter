#include "abstract_formula.h"

struct edge {
  std::vector< int > u;
  std::vector< int > v;
  edge() {
    u = std::vector< int >();
    v = std::vector< int >();
  }
  edge(std::vector< int > a, std::vector< int > b) {
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
  friend bool operator == (const edge& a, const edge& b) {
    return a.u == b.u && a.v == b.v;
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
      if(ret.add_aux_variable(variable(edg, -1, -1).get_name())) {
        ret.add_edge_var(ret.get_name2id().at(variable(edg, -1, -1).get_name()));
      }
      for(int i = 0; i < ins.num_nets; ++i) {
        ret.add_aux_variable(variable(edg, i, -1).get_name());
        for(int j = 0; j < int(ins.nets[i].subnets.size()); ++j) {
          ret.add_variable(variable(edg, i, j).get_name());
        }
      }
    }
  } while(_next(ins, cur));
}

bool abstract_formula::add_to(std::string var_name, std::set< std::string >& s) {
  if(s.count(var_name) == 0) {
    s.insert(var_name);
    int var_id = var_count;
    ++var_count;
    id2name[var_id] = var_name;
    name2id[var_name] = var_id;
    return true;
  }
  return false;
}

bool abstract_formula::add_variable(std::string var_name) {
  return add_to(var_name, variables);
}

bool abstract_formula::add_aux_variable(std::string var_name) {
  return add_to(var_name, aux_variables);
}

void abstract_formula::add_constraint(abstract_constraint::constraint* c) {
  constraints.push_back(c);
}

void abstract_formula::summary(std::ostream& os) {
  os << "Variables: " << variables.size() << std::endl;
  os << "Abstract constraints: " << constraints.size() << std::endl;
}

bool near(std::vector< int >& q, std::vector< std::vector< int > >& endpoints, int k) {
  for(int i = 0; i < int(q.size()); ++i) {
    int l = std::min(endpoints[0][i], endpoints[1][i]);
    int r = std::max(endpoints[0][i], endpoints[1][i]);
    if(l - q[i] > k || q[i] - r > k) {
      return false;
    }
  }
  return true;
}

abstract_formula abstract_formula::from_instance(instance& ins) {
  abstract_formula ret;
  _add_variables(ins, ret);
  // First constraint (1): at most one net per edge
  std::vector< int > cur(ins.num_dims, 1);
  do {
    auto neighbs = edges_from_vertex(ins, cur);
    for(auto& edg : neighbs) {
      if(edg.u == cur) {
        int pe = ret.get_name2id().at(variable(edg, -1, -1).get_name());
        std::vector< int32_t > clause;
        for(int net = 0; net < ins.num_nets; ++net) {
          int pen = ret.get_name2id().at(variable(edg, net, -1).get_name());
          // Coherency constraint: p(e, n) => p(e)
          std::vector< int32_t > impl = {-pen, pe};
          abstract_constraint::constraint *cls = new abstract_constraint::cnfclause(impl);
          ret.add_constraint(cls);
          clause.push_back(pen);
          // Coherency constraint: p(e, n, k) => p(e, n) forall k
          for(int subnet = 0; subnet < int(ins.nets[net].subnets.size()); ++subnet) {
            int pens = ret.get_name2id().at(variable(edg, net, subnet).get_name());
            std::vector< int32_t > impl = {-pens, pen};
            abstract_constraint::constraint *cls = new abstract_constraint::cnfclause(impl);
            ret.add_constraint(cls);
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
        std::vector< int32_t > clause;
        for(auto& edg : neighbs) {
          clause.push_back(ret.get_name2id().at(variable(edg, net, subnet).get_name()));
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
          std::vector< int32_t > clause;
          for(auto& edg : neighbs) {
            clause.push_back(ret.get_name2id().at(variable(edg, net, subnet).get_name()));
          }
          abstract_constraint::constraint *amt = new abstract_constraint::at_most_k(2, clause);
          abstract_constraint::constraint *neo = new abstract_constraint::not_exactly_one(clause);
          ret.add_constraint(amt);
          ret.add_constraint(neo);
        }
      }
    }
  } while(_next(ins, cur));
  // Third constraint (1): If a (u,v) edge is from some net n, then their
  // endpoints cannot have any set edge from any other net
  cur = std::vector< int >(ins.num_dims, 1);
  do {
    auto neighbs = edges_from_vertex(ins, cur);
    for(auto& edg : neighbs) {
      for(int n = 0; n < ins.num_nets; ++n) {
          int32_t e1 = ret.get_name2id().at(variable(edg, n, -1).get_name());
          for(auto& edg2 : neighbs) {
            if(!(edg == edg2)) {
              for(int n2 = 0; n2 < ins.num_nets; ++n2) {
                if(n != n2) {
                  int32_t e2 = ret.get_name2id().at(variable(edg2, n2, -1).get_name());
                  std::vector< int32_t > clause = {-e1, -e2};
                  abstract_constraint::constraint *aib = new abstract_constraint::cnfclause(clause);
                  ret.add_constraint(aib);
                }
              }
            }
          }
        }
      }
  } while(_next(ins, cur));
  // Subnet windowing: restrict possible edges to points inside and or near
  // its bounding box
  cur = std::vector< int >(ins.num_dims, 1);
  do {
    auto neighbs = edges_from_vertex(ins, cur);
    for(auto& edg : neighbs) {
      if(edg.u == cur) {
        for(int net = 0; net < ins.num_nets; ++net) {
          for(int subnet = 0; subnet < int(ins.nets[net].subnets.size()); ++subnet) {
            std::vector< std::vector< int > > endpoints = {
              ins.nets[net].vertices[ins.nets[net].subnets[subnet][0]],
              ins.nets[net].vertices[ins.nets[net].subnets[subnet][1]]
            };
            if((!near(edg.u, endpoints, 2) || !near(edg.v, endpoints, 2))) {
              int var_id = ret.get_name2id().at(variable(edg, net, subnet).get_name());
              std::vector< int32_t > no = {-var_id};
              abstract_constraint::constraint *neg = new abstract_constraint::cnfclause(no);
              ret.add_constraint(neg);
            }
          }
        }
      }
    }
  } while(_next(ins, cur));
  return ret;
}

std::vector< std::vector< int32_t > > abstract_formula::sat_formula(int& first_free) {
  std::vector< std::vector< int32_t > > ret;
  first_free = var_count;
  for(auto constraint : constraints) {
    constraint->to_sat(ret, first_free);
  }
  --first_free;
  return ret;
}

void abstract_formula::print_plottable(std::ostream& os, instance& ins, std::vector< int32_t >& model) {
  std::vector< int > cur(ins.num_dims, 1);
  os << ins.dim_sizes[0] << " " << ins.dim_sizes[1] << std::endl;
  for(auto& var_name : variables) {
    int i = name2id.at(var_name) - 1;
    if(model[i] > 0) {
      os << var_name << std::endl;
    }
  }
  os << "ENDPOINTS" << std::endl;
  for(int i = 0; i < ins.num_nets; ++i) {
    for(auto& v : ins.nets[i].vertices) {
      for(auto& p : v) {
        os << p << " ";
      }
      os << i << std::endl;
    }
  }
}

int abstract_formula::count_used_edges(instance& ins, std::vector< int32_t >& model) {
  // count edges by performing a BFS
  // this way we discard random loops
  std::set< std::vector< int > > vis;
  std::set< edge > s;

  for(int i = 0; i < ins.num_nets; ++i) {
    auto u = ins.nets[i].vertices[0];
    std::queue< std::vector< int > > q;
    q.push(u);
    while(!q.empty()) {
      auto c = q.front();
      q.pop();
      if(vis.count(c) == 0) {
        vis.insert(c);
        for(auto edg : edges_from_vertex(ins, c)) {
          if(model[name2id.at(variable(edg, -1, -1).get_name()) - 1] > 0) {
            s.insert(edg);
            q.push(edg.u);
            q.push(edg.v);
          }
        }
      }
    }
  }
  return s.size();
}

std::vector< int32_t > abstract_formula::unmark_extra_cycles(instance& ins, std::vector< int32_t >& model) {
  std::vector< int32_t > ret(int(model.size()));
  for(int i = 0; i < int(model.size()); ++i) {
    ret[i] = -(i + 1);
  }
  // count edges by performing a BFS
  // this way we discard random loops
  std::set< std::vector< int > > vis;
  std::set< edge > s;
  for(int i = 0; i < ins.num_nets; ++i) {
    auto u = ins.nets[i].vertices[0];
    std::queue< std::vector< int > > q;
    q.push(u);
    while(!q.empty()) {
      auto c = q.front();
      q.pop();
      if(vis.count(c) == 0) {
        vis.insert(c);
        for(auto edg : edges_from_vertex(ins, c)) {
          if(model[name2id.at(variable(edg, -1, -1).get_name()) - 1] > 0) {
            for(int j = 0; j < int(ins.nets[i].subnets.size()); ++j) {
              int varid = name2id.at(variable(edg, i, j).get_name()) - 1;
              if(model[varid] > 0) {
                ret[varid] = varid + 1;
              }
            }
            s.insert(edg);
            q.push(edg.u);
            q.push(edg.v);
          }
        }
      }
    }
  }
  return ret;
}


const std::map< std::string, int >& abstract_formula::get_name2id() {
  return name2id;
}

void abstract_formula::add_edge_var(int32_t id) {
  edge_ids.push_back(id);
}

int abstract_formula::get_var_count() {
  return var_count;
}

std::vector< int32_t > abstract_formula::get_edge_ids() {
  return edge_ids;
}
