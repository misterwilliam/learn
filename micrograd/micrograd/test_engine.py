import micrograd.autodiff

import unittest

class TestValue(unittest.TestCase):

    def test_add(self):
        a = micrograd.autodiff.Value(1)
        b = micrograd.autodiff.Value(2)
        c = a + b
        self.assertEqual(c.data, 3)

        c.backward()
        self.assertEqual(a.grad, 1)
        self.assertEqual(b.grad, 1)

    def test_sub(self):
        a = micrograd.autodiff.Value(1)
        b = micrograd.autodiff.Value(2)
        c = a - b
        self.assertEqual(c.data, -1)

        c.backward()
        self.assertEqual(a.grad, 1)
        self.assertEqual(b.grad, -1)

        d = a + (-b)
        self.assertEqual(d.data, c.data)
        d.backward()
        self.assertEqual(d.grad, c.grad)

    def test_mul(self):
        a = micrograd.autodiff.Value(2)
        b = micrograd.autodiff.Value(3)
        c = a * b
        self.assertEqual(c.data, 6)

        c.backward()
        self.assertEqual(a.grad, 3)
        self.assertEqual(b.grad, 2)

    def test_true_div(self):
        a = micrograd.autodiff.Value(2)
        b = micrograd.autodiff.Value(4)
        c = a / b
        self.assertEqual(c.data, 0.5)

        c.backward()
        self.assertEqual(a.grad, 1/4)
        self.assertEqual(b.grad, 6/16)

    def test_neg(self):
        a = micrograd.autodiff.Value(3)
        b = -a
        self.assertEqual(b.data, -3)

        b.backward()
        self.assertEqual(a.grad, -1)

        c = micrograd.autodiff.Value(-1) * a
        self.assertEqual(c.data, b.data)
        c.backward()
        self.assertEqual(c.grad, b.grad)