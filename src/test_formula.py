from formula import Formula


def main():
    f = Formula()
    f.add_variable('a')
    f.add_variable('b')
    f.add_variable('c')
    f.not_exactly(['a', 'b', 'c'], 1)

    f.print_formula()


if __name__ == '__main__':
    main()
