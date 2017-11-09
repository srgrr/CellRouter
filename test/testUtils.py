import unittest

class testUtils(unittest.TestCase):

    def testIndex2coord(self):
        from utils import index2coord
        dims = [2, 3, 4]
        self.assertEqual((0, 0, 0), index2coord(0, dims))
        self.assertEqual((1, 2, 3), index2coord(2 * 3 * 4 - 1, dims))
        self.assertEqual((1, 0, 0), index2coord(12, dims))


    def testCoord2Index(self):
        from utils import coord2index
        dims = [2, 3, 4]
        self.assertEqual(0, coord2index((0, 0, 0), dims))
        self.assertEqual(2 * 3 * 4 - 1, coord2index((1, 2, 3), dims))
        self.assertEqual(12, coord2index((1, 0, 0), dims))
