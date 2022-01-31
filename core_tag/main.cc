#include <iostream>
#include <string>
#include <vector>

#include "core_tag.h"

int main() {
  // int ok = unshare(CLONE_NEWPID);
  // if (ok != 0) {
  //   std::cout << "unshare errno=" << strerror(errno) << std::endl;
  //   return 0;
  // }

  auto status = core_tag_launcher();
  if (!status.ok()) {
    std::cerr << "Core tag launcher err=" << status << std::endl;
  }

  return 0;
}
