#include <iostream>
#include <string>
#include <vector>

#include "core_tag.h"

int main() {
  std::cout << "Starting thread" << std::endl;
  auto thread_ctx = GetThreadCtx();
  std::cout << DebugString(thread_ctx) << std::endl;

  int ok = unshare(CLONE_NEWPID);
  if (ok != 0) {
    std::cout << "unshare errno=" << strerror(errno) << std::endl;
    return 0;
  }

  return 0;
}
