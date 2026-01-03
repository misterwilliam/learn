"""
Sample multi-threaded implementation of matrix multiply.
"""

import logging
import sys
import torch
import threading

from dataclasses import dataclass

def single_thread_matmul(a, b):
  if a.dim() != 2 or b.dim() != 2:
    raise ValueError("Tensors are not matrices")
  if a.shape[1] != b.shape[0]:
    raise ValueError("tensors shapes not compatible for matmul")
  output = []
  for i in range(a.shape[0]):
    output_row = []
    for j in range(b.shape[1]):
      # Get 0 that matches dtype of input.
      acc = torch.zeros((1,), dtype=a.dtype).item()
      for k in range(a.shape[1]):
        acc += a[i, k].item() * b[k, j].item()
      output_row.append(acc)
    output.append(output_row)
  return torch.tensor(output)

def pytorch_matmul(a, b):
  return a @ b

def run_test_suite():
  @dataclass
  class TestCase:
    name: str
    a: torch.tensor
    b: torch.tensor

  testCases = [
    TestCase("2x2 identity",
             torch.tensor([[1, 0], [0, 1]]),
             torch.tensor([[1, 0], [0, 1]])),
    TestCase("unit vectors",
             torch.tensor([[1, 0]]),
             torch.tensor([[0], [1]])),
    TestCase("Random 5x5",
             torch.rand((1,2)),
             torch.rand((2,1)))
  ]

  def run_test(testCase: TestCase):
    logging.info("TestCase: %s" % testCase.name)
    expected = testCase.a @ testCase.b
    got = single_thread_matmul(testCase.a, testCase.b)
#    got = pytorch_matmul(testCase.a, testCase.b)
    try:
      torch.testing.assert_close(expected, got)
    except AssertionError as e:
      logging.info("Test Fail: %s" % e)

  threads = []
  for i, t in enumerate(testCases):
    # if i != 1:
    #   continue
    t = threading.Thread(target=run_test, args=(t,))
    threads.append(t)
    t.start()

  for t in threads:
    t.join()

def main():
  logging.basicConfig(level=logging.DEBUG,
                      format='(%(asctime)s)[TID=%(thread)d] %(message)s')
  logging.info("Running matmul_multi_thread")
  run_test_suite()
  return 0

if __name__ == "__main__":
  sys.exit(main())
