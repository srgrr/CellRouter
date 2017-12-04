#include "emst.h"

struct edge {
 double w;
 int u, v;
 friend bool operator<(const edge& a, const edge& b) {
   if(std::abs(a.w - b.w) > EPS) {
     return a.w > b.w;
   }
   return random() % 2;
 }
 edge(double d, int a, int b) {
   w = d;
   u = a;
   v = b;
 }
};

double distance(std::vector< int >& a, std::vector< int >& b) {
  double ret = 0.0;
  for(int i = 0; i < int(a.size()); ++i) {
    double va = a[i], vb = b[i];
    ret += (va - vb) * (va - vb);
  }
  return sqrt(ret);
}

int rep(std::vector< int >& v, int u) {
  if(v[u] == u) {
    return u;
  }
  return v[u] = rep(v, v[u]);
}

void join(std::vector< int >& v, int a, int b) {
  v[rep(v, a)] = rep(v, b);
}

void compute_emst_subnet(net& v) {
  int n = int(v.vertices.size());
  std::priority_queue< edge > q;
  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < i; ++j) {
      q.push(edge(distance(v.vertices[i], v.vertices[j]), i, j));
    }
  }
  std::vector< int > component(n);
  for(int i = 0; i < n; ++i) {
    component[i] = i;
  }
  while(!q.empty()) {
    edge cur = q.top();
    q.pop();
    if(rep(component, cur.u) != rep(component, cur.v)) {
      join(component, cur.u, cur.v);
      v.subnets.push_back({cur.u, cur.v});
    }
  }
}

void compute_emst_subnets(instance& ins) {
  for(auto& instance_net : ins.nets) {
    compute_emst_subnet(instance_net);
  }
}
