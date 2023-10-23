#ifndef SKAI_THREADS_FRAMES_ANALYZER_H
#define SKAI_THREADS_FRAMES_ANALYZER_H

#include "threads/thread_context.h"
#include "data_structures/message_queue.h"
#include "cpu_diagnostics/linux.h"

extern frame_func_t analyzer_frame;

typedef struct analyzer_stack {
  stat_cpu_array_t prev;
  stat_cpu_array_t curr;
  stat_cpu_array_t delta;
} analyzer_stack_t;

typedef struct analyzer_context {
  message_queue_t* input;
  message_queue_t* output;
  analyzer_stack_t stack;
} analyzer_context_t;

int analyzer_init(void* context);

int analyzer_loop(void* context);

int analyzer_cleanup(void* context);

#endif
