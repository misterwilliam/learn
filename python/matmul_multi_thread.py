"""
Sample multi-threaded implementation of matrix multiply.
"""

# All the multi-threaded implementations are slower than the single threaded.
# Due to limitations with the Python GIL on 1 thread executes in the Python
# interpreter at a time.
# There don't seem to be consistent performance differences between
# parallelizing the various loops. Benchmarking variance relatively high.
# Presumbably the overhead that Python adds to multi-threading is the dominant
# performance consideration.

import math
import logging
import sys
import time
import torch
import threading

from dataclasses import dataclass


def single_thread_matmul(a: torch.tensor, b: torch.tensor):
  if a.dim() != 2 or b.dim() != 2:
    raise ValueError("Tensors are not matrices")
  if a.shape[1] != b.shape[0]:
    raise ValueError("tensors shapes not compatible for matmul")
  output = torch.zeros((a.shape[0], b.shape[1]), dtype=a.dtype)
  # Get 0 that matches dtype of input.
  zero = torch.zeros((1,), dtype=a.dtype).item()
  for i in range(a.shape[0]):
    for j in range(b.shape[1]):
      acc = zero
      for k in range(a.shape[1]):
        acc += a[i, k].item() * b[k, j].item()
      output[i, j] = acc
  return output

# Parallelize the outermost loop
def multi_thread_matmul_iparallel(a, b, num_threads):
  if a.dim() != 2 or b.dim() != 2:
    raise ValueError("Tensors are not matrices")
  if a.shape[1] != b.shape[0]:
    raise ValueError("tensors shapes not compatible for matmul")

  def _matmul(thread_idx, num_threads, a, b, output):
    # Make each thread responsible for a block of rows in a
    block_size = math.ceil(a.shape[1] / num_threads)
    offset = thread_idx * block_size
    for i in range(offset, offset + block_size):
      if i >= a.shape[0]:
            # Skip when out of bounds
            continue
      for j in range(b.shape[1]):
        # Get 0 that matches dtype of input.
        acc = torch.zeros((1,), dtype=a.dtype).item()
        for k in range(a.shape[1]):
          acc += a[i, k] * b[k, j]
        output[i, j] = acc
    return

  threads = []
  output = torch.zeros((a.shape[0], b.shape[1]), dtype=a.dtype)
  for thread_idx in range(num_threads):
    t = threading.Thread(target=_matmul, args=(thread_idx, num_threads, a, b, output))
    t.start()
    threads.append(t)

  for t in threads:
    t.join()
  return output

# Parallelize the middle loop
def multi_thread_matmul_jparallel(a, b, num_threads):
  if a.dim() != 2 or b.dim() != 2:
    raise ValueError("Tensors are not matrices")
  if a.shape[1] != b.shape[0]:
    raise ValueError("tensors shapes not compatible for matmul")

  def _matmul(thread_idx, num_threads, a, b, output):
    # Make each thread responsible for a block of cols in b
    block_size = math.ceil(b.shape[0] / num_threads)
    offset = thread_idx * block_size
    for i in range(a.shape[0]):
      for j in range(offset, offset + block_size):
        if j >= b.shape[1]:
          # Skip when out of bounds
          continue
        # Get 0 that matches dtype of input.
        acc = torch.zeros((1,), dtype=a.dtype).item()
        for k in range(a.shape[1]):
          acc += a[i, k] * b[k, j]
        output[i, j] = acc
    return

  threads = []
  output = torch.zeros((a.shape[0], b.shape[1]), dtype=a.dtype)
  for thread_idx in range(num_threads):
    t = threading.Thread(target=_matmul, args=(thread_idx, num_threads, a, b, output))
    t.start()
    threads.append(t)

  for t in threads:
    t.join()
  return output

# Parallelize the innermost loop
def multi_thread_matmul_kparallel(a, b, num_threads):
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
    matmul_implementations = [
      (single_thread_matmul, "Single threaded", ()),
      (multi_thread_matmul_iparallel, "Multi-thread (Outermost loop)", (4,)),
      (multi_thread_matmul_jparallel, "Multi-thread (Middle loop)", (4,)),
      (multi_thread_matmul_kparallel, "Multi-thread (Innermost loop)", (4,)),
    ]
    logging.info("- Testing -")
    for matmul_impl, matmul_name, extra_args in matmul_implementations:
      logging.info("-- (%s) Test: %s --", matmul_name, testCase.name)
      expected = testCase.a @ testCase.b
      if extra_args:
        got = matmul_impl(testCase.a, testCase.b, *extra_args)
      else:
        got = matmul_impl(testCase.a, testCase.b)
      try:
        torch.testing.assert_close(expected, got)
      except AssertionError as e:
        logging.info("FAIL: %s" % (testCase.name, e))

  for i, t in enumerate(testCases):
    logging.info("Test Case: %d", i)
    run_test(t)

def do_benchmark():
  n = 120
  a = torch.rand(n, n)
  b = torch.rand(n, n)
  logging.info("- Benchmark %dx%d-" % (n, n))

  matmul_implementations = [
    (pytorch_matmul, "Pytorch", ()),
    (single_thread_matmul, "Single threaded", ()),
    (multi_thread_matmul_iparallel, "Multi-thread (Outermost loop num_threads=2)", (2,)),
    (multi_thread_matmul_iparallel, "Multi-thread (Outermost loop num_threads=4)", (4,)),
    (multi_thread_matmul_jparallel, "Multi-thread (Middle loop num_threads=2)", (2,)),
    (multi_thread_matmul_jparallel, "Multi-thread (Middle loop num_threads=4)", (4,)),
    (multi_thread_matmul_kparallel, "Multi-thread (Innermost loop num_threads=2)", (2,)),
    (multi_thread_matmul_kparallel, "Multi-thread (Innermost loop num_threads=4)", (4,)),
  ]

  for matmul_impl, matmul_name, extra_args in matmul_implementations:
    for i in range(4):
      start_sec = time.monotonic()
      if extra_args:
        matmul_impl(a, b, *extra_args)
      else:
        matmul_impl(a, b)
      logging.info("%s duration: %.3f (secs)", matmul_name, (time.monotonic() - start_sec))


def main():
  logging.basicConfig(level=logging.DEBUG,
                      format='(%(asctime)s)[TID=%(thread)d] %(message)s')
  logging.info("Running matmul_multi_thread")
  run_test_suite()
  do_benchmark()
  return 0

if __name__ == "__main__":
  sys.exit(main())
