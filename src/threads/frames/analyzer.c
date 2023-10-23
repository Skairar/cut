#include "analyzer.h"

#include "logger/logger.h"

frame_func_t analyzer_frame = {
  .init = analyzer_init,
  .loop = analyzer_loop,
  .cleanup = analyzer_cleanup
};


int analyzer_init(void* context) {
  thread_context_t* ctx = context;
  analyzer_context_t* domain = ctx->domain;
  domain->stack.prev = NULL;
  domain->stack.curr = NULL;
  domain->stack.delta = stat_cpu_array_create();
  if (domain->stack.delta == NULL) {
    return -1;
  }
  return 0;
}


int analyzer_loop(void* context) {
  thread_context_t* ctx = context;
  analyzer_context_t* domain = ctx->domain;
  while (timepoint_gt(ctx->loop.end, timepoint_now())) {
    log_puts(log_trace, "<Analyzer> Attempting to fetch input.");
    domain->stack.curr = message_queue_pop_wait_t(domain->input, ctx->loop.end);
    if (domain->stack.curr == NULL) {
      log_puts(log_trace, "<Analyzer> Fetch timed out.");
    } else if (domain->stack.prev != NULL) {
      log_puts(log_trace, "<Analyzer> Input fetched, processing.");
      stat_cpu_percentage_array_t result =
        stat_cpu_percentage_array_create();
      if (result == NULL) {
        //TO DO: Out of memory
        stat_cpu_array_free(domain->stack.prev);
      } else {
        log_puts(log_trace, "<Analyzer> Calculating results.");
        stat_cpu_array_delta(
          domain->stack.prev,
          domain->stack.curr,
          domain->stack.delta
        );
        stat_cpu_percentage_array_calculate(result, domain->stack.delta);
        stat_cpu_array_free(domain->stack.prev);
        int push_flag = message_queue_push(domain->output, result);
        if (push_flag) {
          stat_cpu_percentage_array_free(result);
          log_printf(
            log_error,
            "<Analyzer> Failed to push results, return code: %i.",
            push_flag
          );
        } else {
          log_puts(log_trace, "<Analyzer> Pushed message to queue.");
        }
      }
      domain->stack.prev = domain->stack.curr;
    } else {
      domain->stack.prev = domain->stack.curr;
    }
  }
  return 0;
}


int analyzer_cleanup(void* context) {
  thread_context_t* ctx = context;
  analyzer_context_t* domain = ctx->domain;
  if (domain->stack.prev != NULL) {
    stat_cpu_array_free(domain->stack.prev);
  }
  stat_cpu_array_free(domain->stack.delta);
  return 0;
}
