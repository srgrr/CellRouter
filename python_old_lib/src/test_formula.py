from formula import Formula


def main():
    f = Formula()
    vars = ['a', 'b', 'c', 'd', 'e', 'f']
    for var in vars:
        f.add_variable(var)
    f.at_most_heule([(x, True) for x in vars], 2)
    f.print_formula()


if __name__ == '__main__':
    main()
