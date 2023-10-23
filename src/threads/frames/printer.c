#include "printer.h"

#include "threads/execution_frame.h"
#include "logger/logger.h"
#include "cpu_diagnostics/linux.h"

frame_func_t printer_frame = {
  .init = printer_init,
  .loop = printer_loop,
  .cleanup = printer_cleanup
};


int printer_init(void* context) {
  thread_context_t* ctx = context;
  printer_context_t* domain = ctx->domain;
  domain->stack.cpu_count = stat_layout_get().cpu_count;
  return 0;
}


int printer_loop(void* context) {
  thread_context_t* ctx = context;
  printer_context_t* domain = ctx->domain;
  log_puts(log_trace, "<Printer> Attempting to fetch message.");
  stat_cpu_percentage_array_t result = message_queue_pop_wait_t(
    domain->input,
    ctx->loop.end
  );
  if (result == NULL) {
    //means there's no need for sleep
    log_puts(log_trace, "<Printer> Fetch timed out.");
  } else {
    log_puts(log_trace, "<Printer> Message fetched, printing.");
    puts("\nUsage report:");
    for (size_t i = 1; i < domain->stack.cpu_count; ++i) {
      printf("CPU_core[%zu]_usage = %f%%\n", i - 1, result[i]);
    }
    printf("CPU_total = %f%%\n", result[0]);
    stat_cpu_percentage_array_free(result);
    execution_frame_sleep_until(ctx->loop.end);
  }
  return 0;
}


int printer_cleanup(void* context) {
  (void)context;
  return 0;
}