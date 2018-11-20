#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include "stats.h"

#define SAMPLES_PER_SET 10
#define NUM_SETS 5

void inline benchmark(uint64_t **times) {
  uint64_t start, end;
  unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
  volatile int variable = 0;

  // TODO: Use getrusage to track number of context switches.

  // Intel dialect assembly
  // Run assembly 3 times to put instructions in L1 cache.
  asm volatile("CPUID\n\t"
               "RDTSC\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  asm volatile("RDTSCP\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               "CPUID\n\t"
               : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  asm volatile("CPUID\n\t"
               "RDTSC\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  asm volatile("RDTSCP\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               "CPUID\n\t"
               : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  asm volatile("CPUID\n\t"
               "RDTSC\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  asm volatile("RDTSCP\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               "CPUID\n\t"
               : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "%rcx",
                 "%rdx");

  for (int j = 0; j < NUM_SETS; j++) {
    for (int i = 0; i < SAMPLES_PER_SET; i++) {
      int iters = 10;

      struct rusage rusage_start, rusage_end;
      int ok_rusage_start = getrusage(RUSAGE_SELF, &rusage_start);

      variable = 0;
      asm volatile("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t"
                   : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx",
                     "%rcx", "%rdx");

      // Begin benchmarked code
      // asm volatile("loop: subl $1, %0;"
      //              "jnz loop"
      //              : "=r"(iters)
      //              : "r"(iters));
      asm volatile("subl $1, %0;": "=r"(iters));
      // End benchmarked code

      asm volatile("RDTSCP\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t"
                   "CPUID\n\t"
                   : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx",
                     "%rcx", "%rdx");
      int ok_rusage_end = getrusage(RUSAGE_SELF, &rusage_end);

      if (ok_rusage_end != 0) {
        perror("getrusage(RUSAGE_SELF, &rusage_end)");
        times[j][i] = 0;
        continue;
      }
      if (ok_rusage_start != 0) {
        perror("getrusage(RUSAGE_SELF, &rusage_start)");
        times[j][i] = 0;
        continue;
      }
      int ctx_switches =
          // volunary context switches
          (rusage_end.ru_nvcsw - rusage_start.ru_nvcsw) +
          // involuntary context switches
          (rusage_end.ru_nivcsw - rusage_start.ru_nivcsw);
      printf("Context switches=%ld\n", ctx_switches);
      if (ctx_switches > 0) {
        times[j][i] = 0;
        continue;
      }

      start = (((uint64_t)cycles_high << 32) | cycles_low);
      end = (((uint64_t)cycles_high1 << 32) | cycles_low1);

      if ((end - start) < 0) {
        printf("\n\n>>>>> CRITICAL ERROR IN TAKING THE TIME!!!!!!\n"
               "loop(%d) stat(%d) start = %llu, end = %llu, variable = %u\n",
               j, i, start, end, variable);
        times[j][i] = 0;
      } else {
        times[j][i] = end - start;
        printf("Cycles=%llu\n", end - start);
      }
    }
  }
}

int main(int argc, char **argv) {
  absl::InitializeSymbolizer(argv[0]);
  absl::FailureSignalHandlerOptions options;
  absl::InstallFailureSignalHandler(options);

  uint64_t *variances;
  uint64_t *min_values;
  uint64_t max_dev = 0, min_time = 0, max_time = 0, prev_min = 0, tot_var = 0,
           max_dev_all = 0, var_of_vars = 0, var_of_mins = 0;

  // Allocate memory for times.
  uint64_t **times = (uint64_t **)malloc(NUM_SETS * sizeof(uint64_t *));
  if (!times) {
    printf("Unable to malloc times");
    return EXIT_FAILURE;
  }
  for (int j = 0; j < NUM_SETS; j++) {
    times[j] = (uint64_t *)malloc(SAMPLES_PER_SET * sizeof(uint64_t));
    if (!times[j]) {
      printf("Unable to allocate memory for times[%d]\n", j);
      return EXIT_FAILURE;
    }
  }

  // Allocate memory for variances.
  variances = (uint64_t *)malloc(NUM_SETS * sizeof(uint64_t));
  if (!variances) {
    printf("Unable to allocate memory for variances\n");
    return EXIT_FAILURE;
  }

  // Allocate memory for min values.
  min_values = (uint64_t *)malloc(NUM_SETS * sizeof(uint64_t));
  if (!min_values) {
    printf("Unable to allocate memory for min_values\n");
    return EXIT_FAILURE;
  }

  printf("Benchmarking...\n");
  benchmark(times);

  printf("Calculating stats...\n");
  int spurious = 0;
  for (int j = 0; j < NUM_SETS; j++) {
    double avg = 0;
    max_dev = 0;
    min_time = 0;
    max_time = 0;
    for (int i = 0; i < SAMPLES_PER_SET; i++) {
      if ((min_time == 0) || (min_time > times[j][i]))
        min_time = times[j][i];
      if (max_time < times[j][i])
        max_time = times[j][i];
    }
    max_dev = max_time - min_time;
    min_values[j] = min_time;
    if ((prev_min != 0) && (prev_min > min_time))
      spurious++;
    if (max_dev > max_dev_all)
      max_dev_all = max_dev;
    variances[j] = var_calc(times[j], SAMPLES_PER_SET);
    tot_var += variances[j];
    avg = Avg(times[j], SAMPLES_PER_SET);

    printf("loop_size:%d >>>> avg=%f variance(cycles): %llu; max_deviation: "
           "%llu; min time: %llu\n",
           avg, j, variances[j], max_dev, min_time);
    prev_min = min_time;
  }

  var_of_vars = var_calc(variances, NUM_SETS);
  var_of_mins = var_calc(min_values, NUM_SETS);

  printf("\ntotal number of spurious min values = %d", spurious);
  printf("\ntotal variance = %llu", (tot_var / NUM_SETS));
  printf("\nabsolute max deviation = %llu", max_dev_all);
  printf("\nvariance of variances = %llu", var_of_vars);
  printf("\nvariance of minimum values = %llu\n", var_of_mins);

  for (int j = 0; j < NUM_SETS; j++) {
    free(times[j]);
  }
  free(times);
  free(variances);
  free(min_values);

  return EXIT_SUCCESS;
}
