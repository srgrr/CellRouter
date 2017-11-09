import unittest
from testUtils import testUtils

def main():
	suite = unittest.TestLoader().loadTestsFromTestCase(testUtils)
	#suite.addTest(unittest.TestLoader().loadTestsFromTestCase(testArgsKwargsInstanceMethods))
	unittest.TextTestRunner(verbosity=2).run(suite)

if __name__ == '__main__':
    main()
