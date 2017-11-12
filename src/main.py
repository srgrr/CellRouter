def parse_arguments():
    import argparse
    parser = argparse.ArgumentParser(description='A SAT-Based cell router.')
    parser.add_argument('input_file', type = str, \
                      help = '(Absolute or relative) path to input file')
    parser.add_argument('-oformula', type = str, default = 'output.cnf', \
                      help = '(Absolute or relative) path to output (formula) file')
    parser.add_argument('-omap', type = str, default = 'output.map', \
                      help = '(Absolute or relative) path to dimacs2internal map')
    parser.add_argument('-oplacem', type = str, default = 'output.plc', \
                      help = '(Absolute or relative) path to output (routing) file')
    parser.add_argument('--draw', action = 'store_true', help = 'Plot routing (if found)')
    parser.add_argument('-odraw', type = str, default = 'output.png', \
                      help = '(Absolute or relative) path to output drawing. Only for 2D')
    parser.add_argument('--solver', default = 'picosat', \
                      help = '(Absolute or relative) path to Sat-Solver')
    parser.add_argument('--wirelimit', type = int, nargs = '+', default = [], \
                      help = 'Maximum allowed wire length')
    return parser.parse_args()

def main():
    args = parse_arguments()
    from utils import convert_to_formula
    print('Generating formula...')
    (data_info, formula) = convert_to_formula(args.input_file)
    print('Core formula done.')
    if args.wirelimit:
        assert len(args.wirelimit) == data_info['num_nets'], \
        'Each net must have a wire limit'
        vars = {}
        for net in range(data_info['num_nets']):
            vars[net] = []
        for var in formula.variables:
            vars[int(var.split('-')[-1])].append(var)
        for net in range(data_info['num_nets']):
            print('Adding wirelimit for net %d...'%(net+1))
            formula.at_most_heule([(x,True) for x in vars[net]], args.wirelimit[net])
    print('Printing to dimacs form...')
    formula.print_dimacs(args.oformula, args.omap)
    print('Sat-solving...')
    formula.print_formula()

    import subprocess
    try:
        solver_output = subprocess.check_output(
            [args.solver, args.oformula],
        )
    except Exception as e:
        solver_output = e.output

    import importlib
    output_parser_module = importlib.import_module('%s_parser'%args.solver)
    parsed_output = output_parser_module.parse_output(solver_output)

    if parsed_output is None:
        print('Solver found no solution.')
    else:
        from utils import get_used_vertices
        used_vertices = get_used_vertices(formula, parsed_output)
        print(used_vertices)
        with open(args.oplacem, 'w') as f:
            for vertex in used_vertices:
                f.write(vertex + '\n')
        if args.draw:
            if data_info['num_dims'] == 2:
                print('Plotting...')
                from utils import plot2D
                plot2D(used_vertices, data_info, args.odraw)
            else:
                print('I do not know how to plot this number of dimensions.')
if __name__ == "__main__":
    main()
