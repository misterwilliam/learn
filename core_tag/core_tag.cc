#include "core_tag.h"

#include <pthread.h>

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
    core_tag = *(thread_ctx.core_tag);
  }
  return absl::StrCat("pid=", thread_ctx.pid,
		      " tid=", thread_ctx.tid,
		      " ppid=", thread_ctx.ppid,
		      " core tag=", core_tag);
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

static void* core_tagged_thread(void* arg) {
  ThreadCtx* parent_thread_ctx = (ThreadCtx*) arg;
  
  std::cerr << "In core tagged thread" << std::endl;
  auto thread_ctx = GetThreadCtx();
  std::cerr << DebugString(thread_ctx) << std::endl;
  std::cerr << "Parent thread ctx: " << DebugString(*parent_thread_ctx) << std::endl;

  std::cerr << "Do core tagging" << std::endl;
  auto status = DoCoreTag(0);
  if (!status.ok()) {
    std::cerr << absl::StrCat("DoCoreTag err=", status.ToString()) << std::endl;
  }
  
  thread_ctx = GetThreadCtx();
  std::cerr << DebugString(thread_ctx) << std::endl;

  std::cerr << "Attempt inherit core tag from parent" << std::endl;
  status = InheritCoreTagFromPid(parent_thread_ctx->tid);
  if (!status.ok()) {
    std::cerr << absl::StrCat("InheritFromCoreTagPid err=", status.ToString()) << std::endl;
  }
  thread_ctx = GetThreadCtx();
  std::cerr << DebugString(thread_ctx) << std::endl;

  if (!(parent_thread_ctx->core_tag.ok())) {
    std::cerr << "Parent core tag retrieval err=" << parent_thread_ctx->core_tag.status()
	      << std::endl;
    return nullptr;
  }
  if (!thread_ctx.core_tag.ok()) {
    std::cerr << "Core tag retrieval err=" << thread_ctx.core_tag.status()
	      << std::endl;
    return nullptr;
  }

  if (*(thread_ctx.core_tag) == *(parent_thread_ctx->core_tag)) {
    std::cerr << "INSECURE!! parent core tag inherited" << std::endl;
  } else {
    std::cerr << "SECURE!!!! unable to inherit parent core tag" << std::endl;
  }
  
  return nullptr;
}

absl::Status core_tag_launcher() {
  std::cerr << "In root thread" << std::endl;
  ThreadCtx root_thread_ctx = GetThreadCtx();
  std::cerr << DebugString(root_thread_ctx) << std::endl;
  
  pthread_t thread;
  int _errno = pthread_create(&thread, NULL, core_tagged_thread, &root_thread_ctx);
  if (_errno != 0) {
    return absl::InternalError(absl::StrCat("pthread_create err=", strerror(_errno)));
  }

  std::cerr << "Wait for thread" << std::endl;
  void* ret_val;
  _errno = pthread_join(thread, &ret_val);
  if (_errno != 0) {
    return absl::InternalError(absl::StrCat("pthread_wait err=", strerror(_errno)));
  }
  
  return absl::OkStatus();
}
