#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>

// ThreadCtx is what is passed to the thread.
struct ThreadCtx {
  // thread_id is the thread id of the thread. pthreads has an internal notion
  // thread is that is stored in pthread_t, but we are not allowed to access it.
  // On linux, you could use gettid(), but that is non-portable. For example it
  // is not available on Mac OSX. Therefore we create our own thread id's and
  // pass it in through ThreadCtx.
  uint thread_id;
  // ret_val is memory allocatted for the return value of the thread. The
  // memory must be available after the thread returns so that the parent thread
  // can retrieve the return value, so the eaisiest option is to allocate the
  // memory for the child thread.
  uint ret_val;
};

// ThreadEntry stores book keeping for the parent thread.
struct ThreadEntry {
  // ctx is the ThreadCtx passed to the child thread.
  struct ThreadCtx ctx;
  // thread is the pthread thread identifier of the child thread.
  pthread_t thread;
};

// thread_routine is the function started by pthread_create. Note that the
// function signature is required, even if we don't use args or the return
// value.
void* thread_routine(void* arg) {
  struct ThreadCtx* ctx = (struct ThreadCtx*)arg;
  printf("[thread_id=%u]: Running\n", ctx->thread_id);
  ctx->ret_val = 0;
  return &ctx->ret_val;
}

int main(int argc, char **argv) {
  printf("args:\n");
  for (int i=0; i < argc; ++i) {
    printf("  args[%d]: %s\n", i, argv[i]);
  }

  int NUM_THREADS = 5;
  // Init thread entries
  struct ThreadEntry thread_entries[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; ++i) {
    thread_entries[i].ctx.thread_id = i;
  }

  // Spawn threads
  for (int i = 0; i < NUM_THREADS; ++i) {
    int status = pthread_create(&(thread_entries[i].thread),
                                NULL /* attrs */,
                                thread_routine,
                                &(thread_entries[i].ctx));
    if (status != 0) {
        printf("pthread_create() errno=%d\n", status);
    }
  }

  printf("Hello from main\n");

  // Wait for all threads to complete, and print return value.
  for (int i = 0; i < NUM_THREADS; ++i) {
    void *res;
    int status = pthread_join(thread_entries[i].thread, &res);
    if (status != 0) {
      printf("pthread_join() errno=%d\n", status);
    }
  }

  return 0;
}
