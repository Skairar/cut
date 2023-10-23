#ifndef SKAI_THREADS_FRAMES_READER_H
#define SKAI_THREADS_FRAMES_READER_H

#include "threads/thread_context.h"
#include "data_structures/message_queue.h"

extern frame_func_t reader_frame;

typedef struct reader_context {
  message_queue_t* output;
} reader_context_t;

int reader_init(void* context);

int reader_loop(void* context);

int reader_cleanup(void* context);

#endif
