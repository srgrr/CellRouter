def parse_output(output):
    '''Parse the output of picosat
    '''
    lines = output.decode('utf-8').split('\n')
    result = lines[0].split()[1]
    if result == 'UNSATISFIABLE':
        return None
    # Formula was satisfiable, return an array with a leading zero followed
    # by the values of the variables (- for not, + for positive)
    model = lines[1].split()[1:]
                                    # Remove the trailing zero
    return [0] + list(map(int, model[:len(model) - 1]))
