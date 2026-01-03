"""
Sample Python example showing multi-threaded unittests.
"""

import logging
import sys
import threading

from dataclasses import dataclass

logger = logging.getLogger(__name__)

@dataclass
class TestCase:
  name: str
  x: int
  want: int


class Counter:

  def __init__(self):
    self.i = 0

  def inc(self):
    self.i += 1

c = Counter()
l = threading.Lock()


def foo(a):
  i = 0
  with l:
    c.inc()
    i = c.i
  logging.info("Counter %d" % i)
  return a

def main():
  logging.basicConfig(level=logging.DEBUG,
                      format='(%(asctime)s)[TID=%(thread)d] %(message)s')

  testCases = [
    TestCase("first test", 1, 0),
    TestCase("Test 2", 1, 1),
    TestCase("Test 3", 2, 1),
  ]

  def doTest(testCase):
    logger.info("Running %s" % testCase.name)
    got = foo(testCase.x)
    if got != testCase.want:
      logger.info("Got %s Want: %s" % (got, testCase.want))

  threads = []
  for testCase in testCases:
    t = threading.Thread(target=doTest, args=(testCase,))
    threads.append(t)
    t.start()

  for t in threads:
    t.join()
  return 0

if __name__ == "__main__":
  sys.exit(main())
