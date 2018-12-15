#include <stdbool.h>
#include <stdio.h>
#include "tokenization.h"

int main(int argc, char **argv) {
  char input[] = ",a,bb,,ccc,,";

  struct CharSlice token = Chomp(input, ",");
  while (true) {
    printf("Token (len=%zu)=%.*s\n", token.len, (int)token.len, token.ptr);
    // if token is at the end of string, then stop
    if (token.ptr[token.len] == '\0') {
      break;
    }
    token = Chomp(token.ptr + token.len + 1, ",");
  }

  return 0;
}
