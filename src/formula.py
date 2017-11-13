

class Formula(object):
    '''A SAT formula object. It contains
    all the variables and clauses and can
    be easily converted to DIMACS CNF format
    '''

    def __init__(self):
        self.variables = set()
        # A clause is a list of pairs (var, is_negated)
        self.clauses   = []
        self.implicant = []
        self.aux_count = 0

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
        for (var, sgn) in vars:
            assert var in self.variables, \
            'Variable %s is not in the formula!'%(var)

    def prepend_implicant(self, variable):
        '''Prepends a variable to all future clauses in
        the negated form passed as parameter, meaning that
        this variable implies the following clauses. Can be cleared
        in clear_implicant.
        '''
        (var, value) = variable
        self._check_vars([variable])
        self.implicant = (var, not value)

    def clear_implicant(self):
        '''Clears the implicant. See prepend_implicant.
        '''
        self.implicant = []


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
                to_add = list( (x, not val if neg else val) for (x, val) in selected_vars)
                if self.implicant:
                    to_add = [self.implicant] + to_add
                self.clauses.append(to_add)
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

    def at_most(self, vars, k = 1, neg = True):
        '''At most k constraint. Implemented in the
        naive way, see _at_most method.
        '''
        self._check_vars(vars)
        self._at_most(vars, [], 0, k + 1, True)

    def at_most_heule(self, vars, k = 1, neg = True):
        '''At most k constraint, but a la Heule
        '''
        import sys
        sys.setrecursionlimit(10**9)
        if len(vars) <= k + 3:
            self.at_most(vars, k, neg)
        else:
            aux_vars = ['aux_%d'%(i + self.aux_count) for i in range(k)]
            self.aux_count += k
            for aux_var in aux_vars:
                self.add_variable(aux_var)
            left = vars[:(k+2)] + [(x, True) for x in aux_vars]
            right = [(x, not neg) for x in aux_vars] + vars[(k+2):]
            self.at_most(left, k, neg)
            self.at_most_heule(right, k, neg)

    def at_least(self, vars, k = 1):
        '''At least k constraint. Implemented in the
        naive way. That is, we add all the possible subsets
        of size n - k + 1 of positive literals
        '''
        self._check_vars(vars)
        self._at_most(vars, [], 0, len(vars) - k + 1, False)

    def at_least_heule(self, vars, k = 1):
        '''At least k constraint, but a la Heule
        '''
        self._check_vars(vars)
        self.at_most_heule(vars, len(vars) - k + 1, False)

    def exactly(self, vars, k = 1):
        '''Exactly k constraint. A wrapper to
        at_most k and at_least k, which combined
        form the exactly constraint we want
        '''
        self.at_least(vars, k)
        self.at_most(vars, k)

    def exactly_heule(self, vars, k = 1):
        '''Exactly k constraint, but a la Heule.
        '''
        self.at_least(vars, k)
        self.at_most_heule(vars, k)

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

    def at_most_all_vars(self, k = 1):
        '''At most k of all non-aux variables can be true.
        '''
        vars = []
        for var in self.variables:
            if not 'aux' in var:
                vars.append(var)
        self.at_most_heule([(x, True) for x in vars], k)

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
                for (var, is_true) in clause:
                    first_var = False
                    if not is_true:
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
