#ifndef SKAI_THREADS_FRAMES_LOGGER_H
#define SKAI_THREADS_FRAMES_LOGGER_H

#include "threads/thread_context.h"

extern frame_func_t logger_frame;

int logger_init(void* context);

int logger_loop(void* context);

int logger_cleanup(void* context);

#endif
