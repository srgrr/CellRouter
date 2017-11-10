from formula import Formula


def main():
    f = Formula()
    f.add_variable('a')
    f.add_variable('b')
    f.add_variable('c')
    f.add_variable('d')
    f.not_exactly(['a', 'b', 'c', 'd'], 0)
    f.print_formula()


if __name__ == '__main__':
    main()
