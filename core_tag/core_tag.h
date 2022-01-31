#ifndef CORE_TAG_H_
#define CORE_TAG_H_

#include <sys/prctl.h>

#include <string.h>
#include "absl/status/status.h"
#include "absl/status/statusor.h"

// Bazel can't find <linux/pid.h>. ???
// Head linux has PR_SCHED_CORE_SCOPE_THREAD in sys/prctl.h
#ifndef PIDTYPE_PID
#define PIDTYPE_PID 0
#endif

struct ThreadCtx {
  pid_t pid;
  pid_t tid;
  pid_t ppid;
  absl::StatusOr<unsigned long> core_tag;
};

ThreadCtx GetThreadCtx();
std::string DebugString(const ThreadCtx& thread_ctx);

absl::StatusOr<unsigned long> GetCoreTag(pid_t pid);
absl::Status DoCoreTag(pid_t pid);
absl::Status InheritCoreTagFromPid(pid_t pid);

absl::Status core_tag_launcher();

#endif  // CORE_TAG_H_
