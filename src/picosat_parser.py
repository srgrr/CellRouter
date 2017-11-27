def parse_output(output):
    '''Parse the output of picosat
    '''
    lines = output.decode('utf-8').split('\n')
    _lines = []
    for line in lines:
        if not line.startswith('c'):
            _lines.append(line)
    lines = _lines
    result = lines[0].split()[1]
    if result == 'UNSATISFIABLE':
        return None
    ret = [0]
    for line in lines[1:]:
        ret += list(map(int, line[1:].split()))
    # Formula was satisfiable, return an array with a leading zero followed
    # by the values of the variables (- for not, + for positive)
    model = lines[1].split()[1:]
                                    # Remove the trailing zero
    return ret[:-1]
