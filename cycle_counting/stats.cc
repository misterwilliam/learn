#include "stats.h"

#include <cstdio>
#include <cstdlib>
#include <errno.h>

double Avg(uint64_t *samples, int n) {
  if (n == 0) {
    return 0.0;
  }
  uint64_t acc = 0, prev_acc = 0;
  for (int i = 0; i < n; i++) {
    prev_acc = acc;
    acc += samples[i];
    if (acc < prev_acc) {
      goto overflow;
    }
  }
  return (double)acc / (double)n;

overflow:
  printf("\n\n>>>>>>>>>>>>>> CRITICAL OVERFLOW ERROR IN Avg!!!!!!\n\n");
  return -1.0;
}

uint64_t var_calc(uint64_t *inputs, int size) {
  int i;
  uint64_t acc = 0, previous = 0, temp_var = 0;
  for (i = 0; i < size; i++) {
    if (acc < previous)
      goto overflow;
    previous = acc;
    acc += inputs[i];
  }
  acc = acc * acc;
  if (acc < previous)
    goto overflow;
  previous = 0;
  for (i = 0; i < size; i++) {
    if (temp_var < previous)
      goto overflow;
    previous = temp_var;
    temp_var += (inputs[i] * inputs[i]);
  }
  temp_var = temp_var * size;
  if (temp_var < previous)
    goto overflow;
  temp_var = (temp_var - acc) / (((uint64_t)(size)) * ((uint64_t)(size)));
  return (temp_var);

overflow:
  printf("\n\n>>>>>>>>>>>>>> CRITICAL OVERFLOW ERROR IN var_calc!!!!!!\n\n");
  return -EINVAL;
}
