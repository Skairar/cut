#include "execution_frame.h"

#include "thread_context.h"
#include "logger/logger.h"


int execution_frame(void* context) {
  thread_context_t* ctx = context;
  unsigned char stack_data[ctx->stack_size];
  ctx->stack_data = stack_data;
  ctx->loop.count = 0;
#ifndef EXEC_FRAME_NO_LOG
  log_printf(
    log_trace,
    "<%s> Thread starts.",
    ctx->name
  );
#endif
  if (ctx->frame.init(ctx)) {
    atomic_store(ctx->should_continue, false);
    return -1;
  }
  while (atomic_load(ctx->should_continue)) {
    atomic_store(ctx->watchdog, true);
    ctx->loop.start = timepoint_now();
    ctx->loop.end = timepoint_after(ctx->loop.start, ctx->interval);
    if (ctx->frame.loop(ctx)) {
      break;
    }
    ctx->loop.count += 1;
#ifndef EXEC_FRAME_NO_LOG
    timespan_t total_dur = timespan_dur(ctx->loop.start, timepoint_now());    
    log_printf(
      log_debug,
      "<%s> Iteration #%llx finished in %llisec %llinsec.",
      ctx->name,
      ctx->loop.count,
      (long long int)total_dur.tv_sec,
      (long long int)total_dur.tv_nsec
    );
    if (timepoint_gt(ctx->interval, total_dur)) {
      log_printf(
        log_warning,
        "<%s> Loop finished too fast.",
        ctx->name
      );
    }
#endif
  }
#ifndef EXEC_FRAME_NO_LOG 
  log_printf(
    log_trace,
    "<%s> Thread ends after %llu iterations.",
    ctx->name,
    ctx->loop.count
  );
#endif 
  ctx->frame.cleanup(ctx);
  return 0;
}


void execution_frame_sleep_until(timepoint_t timepoint) {
  timespan_t remaining = timespan_dur(timepoint_now(), timepoint);
  while (timepoint_gt(timepoint, timepoint_now())) {
    thrd_sleep(&remaining, &remaining);
  }
}
