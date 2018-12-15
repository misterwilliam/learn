#include "gtest/gtest.h"
#include "tokenization.h"

// Verify chomp when delimiter is present in middle of input string.
TEST(Chomp, BaseCase) {
  char input[] = "aaa,,,xxx";
  CharSlice s = Chomp(input, ",");
  EXPECT_EQ(s.ptr, input);
  EXPECT_EQ(s.len, 3);
}

// Verify Chomp when delimiter is missing from input string.
TEST(Chomp, DelimeterMissing) {
  char input[] = "aaa";
  CharSlice s = Chomp(input, ",");
  EXPECT_EQ(s.ptr, input);
  EXPECT_EQ(s.len, 3);
}

// Verify Chomp when delimiter is empty set.
TEST(Chomp, EmptyDelimeterSet) {
  char input[] = "aaa";
  CharSlice s = Chomp(input, "");
  EXPECT_EQ(s.ptr, input);
  EXPECT_EQ(s.len, 3);
}

// Verify Chomp when delimiter is at front of input string.
TEST(Chomp, DelimeterAtFront) {
  char input[] = ",,aaa";
  CharSlice s = Chomp(input, ",");
  EXPECT_EQ(s.ptr, input);
  EXPECT_EQ(s.len, 0);
}

// Verify Chomp when input is the empty string.
TEST(Chomp, EmptyInput) {
  char input[] = "";
  CharSlice s = Chomp(input, ",");
  EXPECT_EQ(s.ptr, input);
  EXPECT_EQ(s.len, 0);
}

// Verify example usage when trying to tokenize.
TEST(Chomp, ExampleUsageSimple) {
  char input[] = "cc,b";
  std::vector<CharSlice> tokens;

  struct CharSlice token = Chomp(input, ",");
  while (true) {
    tokens.push_back(token);
    // if token is at the end of string, then stop
    if (token.ptr[token.len] == '\0') {
      break;
    }
    token = Chomp(token.ptr + token.len + 1, ",");
  }

  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0].ptr, input);
  EXPECT_EQ(tokens[0].len, 2);
  EXPECT_EQ(tokens[1].ptr, input + 3);
  EXPECT_EQ(tokens[1].len, 1);
}

// Verify example usage when trying to tokenize string ends withe delimiter
TEST(Chomp, ExampleUsageEndsWithDelimiter) {
  char input[] = "b,";
  std::vector<CharSlice> tokens;

  struct CharSlice token = Chomp(input, ",");
  while (true) {
    tokens.push_back(token);
    // if token is at the end of string, then stop
    if (token.ptr[token.len] == '\0') {
      break;
    }
    token = Chomp(token.ptr + token.len + 1, ",");
  }

  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0].ptr, input);
  EXPECT_EQ(tokens[0].len, 1);
  EXPECT_EQ(tokens[1].ptr, input + 2);
  EXPECT_EQ(tokens[1].len, 0);
}
