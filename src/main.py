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
    parser.add_argument('--optimize', action = 'store_true', \
                      help = 'Optimize wire length')
    parser.add_argument('--encoding', help = 'Encoding generator', \
                      default = 'subset_encoding')
    return parser.parse_args()

def main():
    args = parse_arguments()
    import importlib
    generator_module = importlib.import_module(args.encoding)
    print('Generating formula...')
    (data_info, formula) = generator_module.convert_to_formula(args.input_file)
    print('Generating DIMACS...')
    if args.optimize:
        assert False, 'Feature not implemented!'
    formula.print_dimacs(args.oformula, args.omap)
    print('Sat-solving...')
    # formula.print_formula()

    import subprocess
    try:
        solver_output = subprocess.check_output(
            [args.solver, args.oformula],
        )
    except Exception as e:
        solver_output = e.output

    output_parser_module = importlib.import_module('%s_parser'%args.solver)
    parsed_output = output_parser_module.parse_output(solver_output)

    if parsed_output is None:
        print('Solver found no solution.')
    else:
        used_vertices = [formula.dimacs2internal[x] for x in parsed_output[1:] if x > 0]
        print('Clearing cycles...')
        from utils import clear_cycles
        used_vertices = clear_cycles(data_info, used_vertices)
        with open(args.oplacem, 'w') as f:
            for vertex in used_vertices:
                f.write(vertex + '\n')
        if args.draw:
            if data_info['num_dims'] == 2:
                print('Plotting...')
                from plot2D import plot2D
                plot2D(used_vertices, data_info, args.odraw, \
                args.encoding == 'anti_cycle_encoding')
            else:
                print('I do not know how to plot this number of dimensions.')

if __name__ == "__main__":
    main()
