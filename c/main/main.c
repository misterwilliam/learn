#include <stdio.h>

int main(int argc, char **argv) {
  printf("args:\n");
  for (int i=0; i < argc; ++i) {
    printf("  args[%d]: %s\n", i, argv[i]);
  }
  return 0;
}
