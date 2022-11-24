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
        
    def test_mul(self):
        a = micrograd.engine.Value(2)
        b = micrograd.engine.Value(3)
        c = a * b
        self.assertEqual(c.data, 6)
        
        c.backward()
        self.assertEqual(a.grad, 3)
        self.assertEqual(b.grad, 2)
