#include "instance2formula.h"


/*
  Auxiliary function that gets the adjacent variables (in terms of coordinates) of a variable.
  Two variables are adjacent if they have the same net and their coordinates differ in at most
  one position and with an absolute difference of at most one.
*/
static std::vector< literal> get_neighbor_variables(const instance& ins, const variable& var) {
  std::vector< literal > ret;
  variable adj = var;
  for(int i = 0; i < ins.num_dims; ++i) {
    for(int j = -1; j <= 1; j += 2) {
      adj.coords[i] += j;
      if(ins.var2index.count(adj) > 0) {
        ret.push_back(ins.var2index.at(adj));
      }
      adj.coords[i] -= j;
    }
  }
  return ret;
}

std::vector< std::vector< literal > > generate_basic_formula(const instance& ins) {
  PB2CNF pb2cnf;
  PBConfig config = std::make_shared< PBConfigClass >();
  VectorClauseDatabase formula(config);
  // Indices of aux variables will start from ins.allowed_vars.size + 1
  literal first_free_variable = literal(ins.allowed_variables.size()) + 1;
  
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
    formula.addClause(std::vector< literal >(1, source_lit));
    formula.addClause(std::vector< literal >(1, sink_lit));
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
    while(start != ins.allowed_variables.end() && (*start).coords == var.coords) {
      clause_components.push_back(ins.var2index.at(*start));
      ++start;
    }
    std::vector< std::vector< literal > > _formula;
    // Create an at most one constraint with all these variables
    first_free_variable = pb2cnf.encodeAtMostK(clause_components, 1ll, _formula, first_free_variable) + 1;
    for(auto& clause : _formula) {
      formula.addClause(clause);
    }
  }

  // Constraint 2.1:
  // An endpoint has exactly 1 neighbor
  std::set< variable > endpoints;
  for(int i = 0; i < ins.num_nets; ++i) {
    const auto& source_sink = ins.points[i];
    for(const auto& endpoint : source_sink) {
      const auto adj_vertices = get_neighbor_variables(ins, variable(endpoint, i));
      std::vector< std::vector< literal > > _formula;
      first_free_variable = pb2cnf.encodeAtLeastK(adj_vertices, 1ll, _formula, first_free_variable) + 1;
      first_free_variable = pb2cnf.encodeAtMostK(adj_vertices, 1ll, _formula, first_free_variable) + 1;
      for(auto& clause : _formula) {
        formula.addClause(clause);
      }
      endpoints.insert(variable(endpoint, i));
    }
  }
  // Constraint 2.2:
  // A set non-endpoint vertex IMPLIES two set neighbors
  for(auto& var : ins.allowed_variables) {
    if(endpoints.count(var)) continue;
    auto var_index = ins.var2index.at(var);
    const auto adj_vertices = get_neighbor_variables(ins, var);
    std::vector< std::vector< literal > > _formula;
    first_free_variable = pb2cnf.encodeAtLeastK(adj_vertices, 2ll, _formula, first_free_variable) + 1;
    first_free_variable = pb2cnf.encodeAtMostK(adj_vertices, 2ll, _formula, first_free_variable) + 1;
    for(auto& clause : _formula) {
      // Add implicant by hand
      clause.push_back(- var_index);
      formula.addClause(clause);
    }
  }

  std::cout << "p cnf " << first_free_variable << " " << formula.getClauses().size() << std::endl;
  for(const auto& clause : formula.getClauses()) {
    for(auto& lit : clause) {
      std::cout << lit << " ";
    }
    std::cout << 0 << std::endl;
  }
  return formula.getClauses();
}


