#ifndef SKAI_THREADS_FRAMES_PRINTER_H
#define SKAI_THREADS_FRAMES_PRINTER_H

#include "threads/thread_context.h"
#include "data_structures/message_queue.h"

extern frame_func_t printer_frame;

typedef struct printer_stack {
  size_t cpu_count;
} printer_stack_t;

typedef struct printer_context {
  message_queue_t* input;
  printer_stack_t stack;
} printer_context_t;

int printer_init(void* context);

int printer_loop(void* context);

int printer_cleanup(void* context);

#endif
