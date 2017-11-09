import argparse
import subprocess
from utils import convert_to_formula

def parse_arguments():
    parser = argparse.ArgumentParser(description=open('README.txt').read())
    parser.add_argument('input_file', type = str, \
                      help = '(Absolute or relative) path to input file')
    parser.add_argument('-oformula', type = str, default = 'output.cnf', \
                      help = '(Absolute or relative) path to output (formula) file')
    parser.add_argument('-omap', type = str, default = 'output.map', \
                      help = '(Absolute or relative) path to internal2dimacs map')
    parser.add_argument('-oplacem', type = str, default = 'output.plc', \
                      help = '(Absolute or relative) path to output (routing) file')
    parser.add_argument('--draw', action = 'store_true', help = 'Plot routing (if found)')
    parser.add_argument('--solver', default = 'picosat', \
                      help = '(Absolute or relative) path to Sat-Solver')
    return parser.parse_args()

def main():
    args = parse_arguments()
    formula = convert_to_formula(args.input_file)
    formula.print_dimacs(args.oformula, args.omap)
    print(solver_output)

if __name__ == "__main__":
    main()
