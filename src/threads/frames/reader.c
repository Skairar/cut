#include "reader.h"

#include <stdio.h>

#include "threads/execution_frame.h"
#include "logger/logger.h"
#include "cpu_diagnostics/linux.h"


frame_func_t reader_frame = {
  .init = reader_init,
  .loop = reader_loop,
  .cleanup = reader_cleanup
};


int reader_init(void* context) {
  (void)context;
  return 0;
}


int reader_loop(void* context) {
  thread_context_t* ctx = context;
  reader_context_t* domain = ctx->domain;
  int read_flag = 0;
  int push_flag = 0;
  FILE* proc_stat = fopen("/proc/stat", "r");
  if (proc_stat == NULL) {
    log_puts(log_fatal, "<Reader> Failed to open /proc/stat.");
    return -1;
  }
  stat_cpu_array_t data = stat_cpu_array_create();
  if (data != NULL) {
    read_flag = stat_cpu_array_read_f(data, proc_stat);
    fclose(proc_stat);
  } else {
      //TO DO: out of memory condition
    fclose(proc_stat);
    return 0;
  }
  if (read_flag == 0) {
    push_flag = message_queue_push(domain->output, data);
    log_puts(log_trace, "<Reader> Pushed message to queue.");
  } else {
    stat_cpu_array_free(data);
    log_printf(
      log_error,
      "<Reader> Read from /stat/cpu failed, return code: %i.",
      read_flag
    );
  }
  if (push_flag) {
    stat_cpu_array_free(data);
    log_printf(
      log_error, 
      "<Reader> Failed to push results, return code: %i.",
      push_flag
    );
  }
  execution_frame_sleep_until(ctx->loop.end);
  return 0;
}


int reader_cleanup(void* context) {
  (void)context;
  return 0;
}
