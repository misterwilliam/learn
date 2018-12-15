Tokenizing a string by using strcspn, instead of strtok. This is better than
using strtok because strtok requiring copying the string being tokenized into a
global variable.
