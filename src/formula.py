

class Formula(object):
    '''A SAT formula object. It contains
    all the variables and clauses and can
    be easily converted to DIMACS CNF format
    '''

    def __init__(self):
        self.variables = set()
        # A clause is a list of pairs (var, is_negated)
        self.clauses   = []

    def add_variable(self, var):
        '''Adds a variable
        '''
        assert not var in self.variables, \
        'Variable %s is already added'%var
        self.variables.add(var)

    def _check_vars(self, vars):
        '''Check that all vars in a given list
        belong to the formula
        '''
        for var in vars:
            assert var in self.variables, \
            'Variable %s is not in the formula!'%(var)


    def _at_most(self, vars, selected_vars, index, k, neg = False):
        '''Backtracking procedure that generates and
        adds all clauses needed to implement an
        at most k constraint in the naive way.
        That is, if we want to implement an at most k
        then we have to generate all the n choose k+1
        clauses consisting of k+1 negated variables.
        These clauses impose that we can choose no subset
        of k+1 variables. This also imposes that no bigger
        subsets can be chosen, given that any bigger subset
        will have some of the clause subsets included
        '''
        if index == len(vars):
            if k == 0:
                self.clauses.append(
                    list(
                        (x, not neg) for x in selected_vars
                    )
                )
            return
        # Prune: do we have enough variables to add?
        if len(vars) - index < k:
            return
        # Ignore current variable
        self._at_most(vars, selected_vars, index + 1, k, neg)
        # Add current variable
        selected_vars.append(vars[index])
        self._at_most(vars, selected_vars, index + 1, k - 1, neg)
        # Pop stack to leave it intact
        selected_vars.pop()

    def at_most(self, vars, k = 1):
        '''At most k constraint. Implemented in the
        naive way, see _at_most method.
        '''
        self._check_vars(vars)
        self._at_most(vars, [], 0, k + 1, False)

    def at_least(self, vars, k = 1):
        '''At least k constraint. Implemented in the
        naive way. That is, we add all the possible subsets
        of size n - k + 1 of positive literals
        '''
        self._check_vars(vars)
        self._at_most(vars, [], 0, len(vars) - k + 1, False)

    def exactly(self, vars, k = 1):
        '''Exactly k constraint. A wrapper to
        at_most k and at_least k, which combined
        form the exactly constraint we want
        '''
        self.at_least(vars, k)
        self.at_most(vars, k)

    def _not_exactly(self, vars, negated, index, k = 1):
        if index == len(vars):
            if k == 0:
                to_add = []
                for i in range(len(vars)):
                    to_add.append(
                        list(
                            (vars[i], not negated[i])
                        )
                    )
                self.clauses.append(to_add)
            return
        # Prune: can we negate enough variables?
        if len(vars) - index < k:
            return
        # Ignore this variable
        self._not_exactly(vars, negated, index + 1, k)
        # Negate this variable
        negated[index] = True
        self._not_exactly(vars, negated, index + 1, k - 1)
        negated[index] = False

    def not_exactly(self, vars, k = 1):
        '''NOT exactly k constraint.
        A not exactly 1 is not hard to write as
        (!( a ^  !b ^ !c))
        (!(!a ^   b ^ !c))
        (!(!a ^  !b ^  c))
        We can see that we can apply De Morgan's
        laws to convert it to CNF:
        !a v  b v  c
         a v !b v  c
         a v  b v !c
        And we see that it is not hard to generalize
        to the k case: apply De Morgan's laws to all
        the possible (n choose k) conjunctions that
        have exactly k negated variables.
        It is very similar to the at most and at least
        constraints except for that we need to include
        all the involved variables in each clause, not
        only the "marked" to belong to the subset.
        See _not_exactly method.
        '''
        self._not_exactly(vars, [False]*len(vars), 0, k)

    def print_formula(self):
        '''Prints the formula to the standard output
        with the internal variable names
        '''
        for clause in self.clauses:
            print(' '.join(('+' if t else '-') + n for (n, t) in clause))

    def assign_dimacs_id(self):
        current_id = 1
        self.internal2dimacs = {}
        self.dimacs2internal = {}
        for var in self.variables:
            self.internal2dimacs[var] = current_id
            self.dimacs2internal[current_id] = var
            current_id += 1


    def print_dimacs(self, output_dimacs, output_dimacs_map):
        '''Prints the formula in DIMACS CNF format
        '''
        self.assign_dimacs_id()
        with open(output_dimacs_map, 'w') as f:
            import json
            json.dump(self.dimacs2internal, f)
        with open(output_dimacs, 'w') as f:
            f.write('c Generated by CellRouter, look at JSON variable remapping\n')
            f.write('p cnf %d %d\n'%(len(self.variables), len(self.clauses)))
            for clause in self.clauses:
                for (var, is_negated) in clause:
                    first_var = False
                    if is_negated:
                        f.write('-')
                    f.write(str(self.internal2dimacs[var]) + ' ')
                f.write('0 \n')

    def add_clause(self, clause):
        '''Adds a single clause to the formula
        '''
        assert isinstance(clause, list) or isinstance(clause, tuple), \
        'Clause must be a list or a tuple'
        for var in clause:
            assert len(var) == 2 and isinstance(var[0], str) and \
            isinstance(var[1], bool), \
            'Wrong clause format, it must be of the form (varid, is_negated). Got\
            %s'%var
            assert var[0] in self.variables, \
            'Variable %s is not in the formula'%var[0]
        self.clauses.append(clause)
