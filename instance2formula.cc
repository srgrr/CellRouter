#include "instance2formula.h"


/*
	Auxiliary function that gets the adjacent variables (in terms of coordinates) of a variable.
	Two variables are adjacent if they have the same net and their coordinates differ in at most
	one position and with an absolute difference of at most one.
*/
static std::vector< literal> get_neighbor_variables(const instance& ins, const variable& var) {
  std::vector< variable > ret;
  variable adj = var;
  for(int i = 0; i < ins.num_dims; ++i) {
    for(int j = -1; j <= 1; j += 2) {
      adj.coords[i] += j;
      if( adj.coords[i] > 0 && adj.coords[i] <= ins.dim_sizes[i] ) {
        ret.push_back(ins.var2index.at(adj));
      }
      adj.coords[i] -= j,
    }
  }
  return ret;
}

std::vector< std::vector< literal > > generate_basic_formula(const instance& ins) {
  PB2CNF pb2cnf;
  std::vector< std::vector< literal > > formula;
  // Indices of aux variables will start from ins.allowed_vars.size + 1
  literal first_fresh_variable = literal(ins.allowed_variables.size()) + 1;
  
  // As an optimization, we can just iterate through the
  // set of allowed variables, and only add the necessary ones
  // to each constraint. We can also keep track of the already
  // treated coordinates in order to avoid putting repeated constraints.
  // See how this set is used in constraints 1.1 and 1.2
  std::set< std::vector< int > > processed_vertices;

  // Constraint 1.1:
  // Given an endpoint, set it to true.
  // That is, force endpoints to be set.
  // This, in conjunction with other constraints, will force the solver to
  // compute full paths for all net pairs (see future constraints).
  for(int i = 0; i < ins.num_nets; ++i) {
    const auto& source_sink  = ins.points[i];
    const auto& source = source_sink[0];
    const auto& sink   = source_sink[1];
    literal source_lit = ins.var2index.at(variable(source, i));
    literal sink_lit   = ins.var2index.at(variable(sink, i));
    // We can add these clauses by hand, as they are unit clauses
    formula.push_back(std::vector< literal >(1, source_lit));
    formula.push_back(std::vector< literal >(1, sink_lit));
    // Add source and sink coordinates to the set of used vertices in order to
    // avoid processing them again in constraint 1.2
    processed_vertices.insert(source);
    processed_vertices.insert(sink);
  }

  // Constraint 1.2:
  // Given coordinates (x1, x2, ..., xd)
  // At most one of the (x1, x2, ..., xd, nk)
  // is true.
  // That is, each vertex contains at most one net.
  for(const variable& var : ins.allowed_variables) {
    // Check if coordinates were already processed
    // if not, add them to the already processed set and proceed
    if(processed_vertices.count(var.coords)) continue;
    processed_vertices.insert(var.coords);
    std::vector< literal > clause_components;
    // Just pick all variables that have the same coordinates, but
    // different nets, and remember their var indices, as we only need
    // these for constraints encoding.
    auto start = ins.allowed_variables.lower_bound(variable(var.coords, 0));
    while(start != ins.allowed_variables.end() && (*start).net == var.net) {
      clause_components.push_back(ins.var2index.at(*start));
      ++start;
     }
    // Create an at most one constraint with all these variables
    first_fresh_variable = pb2cnf.encodeAtMostK(clause_components, 1ll, formula, first_fresh_variable) + 1;
  }

  // Constraint 2.1:
  // An endpoint has exactly 1 neighbor
  std::set< variable > endpoints;
  for(int i = 0; i < ins.num_nets; ++i) {
    const auto& source_sink = ins.points[i];
    for(auto& endpoint : source_sink) {
      const auto adj_vertices = get_neighbor_variables(ins, endpoint);
      //TODO: Discover if this formula would be better encoded as a PBC
      first_fresh_variable = pb2cnf.encodeAtMostK(adj_vertices, 1ll, formula, first_fresh_variable) + 1;
      first_fresh_variable = pb2cnf.encodeAtLeastK(adj_vertices, 1ll, formula, first_fresh_variable) + 1;
    }
  }
  // Constraint 2.2:
  // A set non-endpoint vertex IMPLIES two set neighbors
	//TODO: Implement this constraint
	//TODO: Use pb2cnf.addConditional(literal) [ WITH SIGN!!! ]

  for(auto& clause : formula) {
    for(auto& lit : clause) {
      std::cout << lit << " ";
    }
    std::cout << std::endl;
  }

  return formula;
}


