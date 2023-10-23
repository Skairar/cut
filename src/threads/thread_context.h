#ifndef SKAI_THREADS_THREAD_CONTEXT_H
#define SKAI_THREADS_THREAD_CONTEXT_H

#include <stdatomic.h>

#include "utilities/time.h"

typedef int (*init_func)(void*);
typedef int (*loop_func)(void*);
typedef int (*cleanup_func)(void*);

typedef struct frame_func {
  init_func init;
  loop_func loop;
  cleanup_func cleanup;
} frame_func_t;

typedef struct loop_context {
  timepoint_t start;
  timepoint_t end;
  unsigned long long int count;
} loop_context_t;

typedef struct thread_context {
  loop_context_t loop;
  frame_func_t frame;
  timespan_t interval;
  const char* name;
  size_t stack_size;
  unsigned char* stack_data;
  void* domain;
  atomic_bool* should_continue;
  atomic_bool* watchdog;
} thread_context_t;


#endif
