#include <stdio.h>
#include <string.h>

struct CharSlice {
  char* ptr;
  size_t len;
};

// Returns a CharSlice of the largest consecutive sequence of chars not in the
// delimiter set. If input is the empty string, returns a CharSlice pointed to
// that string with length 0.
struct CharSlice Chomp(char *input, const char* delim) {
  struct CharSlice ret;

  // If at the end return a 0 length slice pointed to NULL.
  if (input[0] == '\0') {
    ret.ptr = input;
    ret.len = 0;
    return ret;
  }

  // Return slice of the largest consecutive sequence of chars not
  // in the delimiter set.
  ret.ptr = input;
  ret.len = strcspn(input, delim);

  return ret;
}
