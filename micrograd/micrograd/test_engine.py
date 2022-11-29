import micrograd.engine

import unittest

class TestValue(unittest.TestCase):
    
    def test_add(self):
        a = micrograd.engine.Value(1)
        b = micrograd.engine.Value(2)
        c = a + b
        self.assertEqual(c.data, 3)
        
        c.backward()
        self.assertEqual(a.grad, 1)
        self.assertEqual(b.grad, 1)
        
    def test_sub(self):
        a = micrograd.engine.Value(1)
        b = micrograd.engine.Value(2)
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
        a = micrograd.engine.Value(2)
        b = micrograd.engine.Value(3)
        c = a * b
        self.assertEqual(c.data, 6)
        
        c.backward()
        self.assertEqual(a.grad, 3)
        self.assertEqual(b.grad, 2)

    def test_neg(self):
        a = micrograd.engine.Value(3)
        b = -a
        self.assertEqual(b.data, -3)
        
        b.backward()
        self.assertEqual(a.grad, -1)
        
        c = micrograd.engine.Value(-1) * a
        self.assertEqual(c.data, b.data)
        c.backward()
        self.assertEqual(c.grad, b.grad)