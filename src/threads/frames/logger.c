#include "logger.h"

#include "threads/execution_frame.h"
#include "logger/logger.h"

frame_func_t logger_frame = {
  .init = logger_init,
  .loop = logger_loop,
  .cleanup = logger_cleanup
};


int logger_init(void* context) {
  (void)context;
  return 0;
}


int logger_loop(void* context) {
  thread_context_t* ctx = context;
  log_process_some_dur(ctx->interval);
  return 0;
}


int logger_cleanup(void* context) {
  thread_context_t* ctx = context;
  execution_frame_sleep_until(timepoint_after(timepoint_now(), ctx->interval));
  log_process_all();
  return 0;
}
