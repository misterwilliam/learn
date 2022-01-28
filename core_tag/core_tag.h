#include <sys/prctl.h>

#include <string.h>
#include "absl/status/status.h"
#include "absl/status/statusor.h"

// Bazel can't find <linux/pid.h>. ???
#ifndef PIDTYPE_PID
#define PIDTYPE_PID 0
#endif

absl::StatusOr<unsigned long> GetCoreTag(pid_t pid) {
  unsigned long cookie;
  if (prctl(PR_SCHED_CORE, PR_SCHED_CORE_GET, pid, PIDTYPE_PID, &cookie) < 0) {
    return absl::InternalError(strerror(errno));
  }

  return absl::OkStatus();
}
