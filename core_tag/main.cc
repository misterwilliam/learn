#include <iostream>
#include <string>
#include <vector>

#include "core_tag.h"

int main() {
  auto core_tag = GetCoreTag(0);
  if (!core_tag.ok()) {
    std::cout << core_tag.status();
  } else {
    std::cout << "Core Tag: " << *core_tag;
  }

  return 0;
}
