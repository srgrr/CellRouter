from formula import Formula


def main():
    f = Formula()
    f.add_variable('a')
    f.add_variable('b')
    f.add_variable('c')
    f.at_least(['a', 'b', 'c'], 3)
    f.print_formula()


if __name__ == '__main__':
    main()
