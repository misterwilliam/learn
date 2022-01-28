#include "core_tag.h"

#include "absl/strings/str_cat.h"

ThreadCtx GetThreadCtx() {
  return ThreadCtx {
    .pid = getpid(),
    .tid = gettid(),
    .ppid = getppid(),
    .core_tag = GetCoreTag(0),
  };
}

std::string DebugString(const ThreadCtx& thread_ctx) {
  int64_t core_tag = -1;
  if (thread_ctx.core_tag.ok()) {
    core_tag = *thread_ctx.core_tag;
  }
  return absl::StrCat("pid=", thread_ctx.pid,
		      " tid=", thread_ctx.tid,
		      " ppid=", thread_ctx.ppid,
		      " core tag=", core_tag,
		      "\n");
}

absl::StatusOr<unsigned long> GetCoreTag(pid_t pid) {
  unsigned long cookie;
  if (prctl(PR_SCHED_CORE, PR_SCHED_CORE_GET, pid, PIDTYPE_PID, &cookie) < 0) {
    return absl::InternalError(strerror(errno));
  }

  return cookie;
}

absl::Status DoCoreTag(pid_t pid) {
  // cookie argument must be nullptr.
  if (prctl(PR_SCHED_CORE, PR_SCHED_CORE_CREATE, pid, PIDTYPE_PID, nullptr) <
      0) {
    return absl::InternalError(strerror(errno));
  }

  return absl::OkStatus();
}

absl::Status InheritCoreTagFromPid(pid_t pid) {
  if (prctl(PR_SCHED_CORE, PR_SCHED_CORE_SHARE_FROM, pid, PIDTYPE_PID,
            nullptr) < 0) {
    return absl::InternalError(strerror(errno));
  }

  return absl::OkStatus();
}
