# Cycle counting

Status: incomplete

Adapting cycle counting techniques from [1] and [2], so that the benchmark
happens in userspace instead of a kernel module. To compensate for the fact that
interrupts can happen, samples that have context switches as reported by
`getrusage` are ignored.

## References

[1] [How to Benchmark Code Execution Times on Intel® IA-32 and IA-64 Instruction Set Architectures](https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf)
Intel Sept 2010

[2] [Using the RDTSC Instruction for Performance Monitoring](https://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf)
Intel 1997

[3] [Intel® 64 and IA-32 Architectures Software Developer's Manual Combined Volumes 3A, 3B, 3C, and 3D: System Programming Guide]https://software.intel.com/sites/default/files/managed/a4/60/325384-sdm-vol-3abcd.pdf
Intel 2018
Chapter 17.17 "TIME_-STAMP COUNTER" documents time-stamp counter behavior.
