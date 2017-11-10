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
    parser.add_argument('--solver', default = 'picosat', \
                      help = '(Absolute or relative) path to Sat-Solver')
    return parser.parse_args()

def main():
    args = parse_arguments()
    from utils import convert_to_formula
    (data_info, formula) = convert_to_formula(args.input_file)
    formula.print_dimacs(args.oformula, args.omap)

    import subprocess
    try:
        solver_output = subprocess.check_output(
            [args.solver, args.oformula],
        )
    except Exception, e:
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
if __name__ == "__main__":
    main()
