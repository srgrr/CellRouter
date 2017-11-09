A cell router that uses SAT

Format for input cases:

numnets numdim dim1 ... dimn
source1coord1 ... source1coordnumdim sink1coord1 ... sink1coordnumdim
...
sourcencoord1 ... sourcencoordnumdim sinkncoord1 ... sinkncoordnumdim

All coordinates must be integers, as they will serve
to indicate the position of net endpoints in a discretized grid.

This program does two things:
1) It creates and stores the resulting CNF formula
2) It solves via some SatSolver (by default, picosat) the generated formula
   and stores the resulting placement. It can also, optionally, plot it
   for the two dimensional case.


Dependencies:

Python 3.x - although it is more than likely that this also works for Py2.7

Some sat solver that accepts formulas in DIMACS CNF format
