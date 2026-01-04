"""
Sample multi-threaded implementation of matrix multiply.
"""

import math
import logging
import sys
import time
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

def multi_thread_matmul(a, b, num_threads):
  if a.dim() != 2 or b.dim() != 2:
    raise ValueError("Tensors are not matrices")
  if a.shape[1] != b.shape[0]:
    raise ValueError("tensors shapes not compatible for matmul")

  def _matmul(thread_idx, num_threads, a, b, output):
    # Make each thread responsible to a block from each row in a, and a block
    # from each col in b.
    block_size = math.ceil(a.shape[1] / num_threads)
    offset = thread_idx * block_size
    for i in range(a.shape[0]):
      for j in range(b.shape[1]):
        # Get 0 that matches dtype of input.
        acc = torch.zeros((1,), dtype=a.dtype).item()
        for k in range(offset, offset + block_size):
          if k >= a.shape[1]:
            # Skip when out of bounds
            continue
          acc += a[i, k] * b[k, j]
        output[i, j] = acc
    return

  threads = []
  tiles = []
  for thread_idx in range(num_threads):
    tile = torch.zeros((a.shape[0], b.shape[1]), dtype=a.dtype)
    tiles.append(tile)
    t = threading.Thread(target=_matmul, args=(thread_idx, num_threads, a, b, tile))
    t.start()
    threads.append(t)

  for t in threads:
    t.join()
  output = torch.zeros((a.shape[0], b.shape[1]), dtype=a.dtype)
  for i in range(a.shape[0]):
    for j in range(b.shape[1]):
      output[i, j] = sum(tile[i, j] for tile in tiles)
  return output


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
    TestCase("Orthogonal unit vectors",
             torch.tensor([[1, 0]]),
             torch.tensor([[0], [1]])),
    TestCase("Ones 2x2 @ 2x2",
             torch.ones((2, 2)),
             torch.ones((2, 2))),
    TestCase("Ones 2x3 @ 3x2",
             torch.ones((2, 3)),
             torch.ones((3, 2))),
    TestCase("Ones 5x7 @ 7x4",
             torch.ones((5, 7)),
             torch.ones((7, 4))),
    TestCase("Random 5x7 @ 7x4",
             torch.rand((5, 7)),
             torch.rand((7, 4)))
  ]

  def run_test(testCase: TestCase):
    logging.info("- Testing -")
    logging.info("-- (Singlethread) Test: %s --" % testCase.name)
    expected = testCase.a @ testCase.b
    start_sec = time.monotonic()
    got = single_thread_matmul(testCase.a, testCase.b)
    logging.info("Duration: %.3f (secs)" % (time.monotonic() - start_sec))
    try:
      torch.testing.assert_close(expected, got)
    except AssertionError as e:
      logging.info("FAIL: %s" % (testCase.name, e))
    else:
      logging.info("PASS")

    logging.info("-- (Multi-thread) Test: %s --" % testCase.name)
    expected = testCase.a @ testCase.b
    start_sec = time.monotonic()
    got = multi_thread_matmul(testCase.a, testCase.b, 7)
    logging.info("Duration: %.3f (secs)" % (time.monotonic() - start_sec))
    try:
      torch.testing.assert_close(expected, got)
    except AssertionError as e:
      logging.info("FAIL: %s" % (testCase.name, e))
    else:
      logging.info("PASS")

  threads = []
  for i, t in enumerate(testCases):
    # if i != 0:
    #   continue
    run_test(t)

def do_benchmark():
  n = 100
  a = torch.rand(n, n)
  b = torch.rand(n, n)
  logging.info("- Benchmark %dx%d-" % (n, n))

  start_sec = time.monotonic()
  pytorch_matmul(a, b)
  logging.info("Pytorch duration: %.3f (secs)" % (time.monotonic() - start_sec))

  start_sec = time.monotonic()
  single_thread_matmul(a, b)
  logging.info("Single thread duration: %.3f (secs)" % (time.monotonic() - start_sec))

  start_sec = time.monotonic()
  threads = 2
  multi_thread_matmul(a, b, threads)
  logging.info("Multi-thread (threads=%d) duration: %.3f (secs)" % (threads, time.monotonic() - start_sec))

def main():
  logging.basicConfig(level=logging.DEBUG,
                      format='(%(asctime)s)[TID=%(thread)d] %(message)s')
  logging.info("Running matmul_multi_thread")
  run_test_suite()
  do_benchmark()
  return 0

if __name__ == "__main__":
  sys.exit(main())
