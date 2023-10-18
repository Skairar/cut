#include "logger.h"

#include <string.h>
#include <threads.h>
#include <time.h>
#include <stdarg.h>

#include "data_structures/queue.h"
#include "message_queue.h"
#include "output_sinks.h"
#include "utilities/string.h"
#include "utilities/time.h"


typedef struct log_config {
  time_t start_time;
  enum log_severity min_severity;
} log_config_t;


typedef struct log_context {
  log_config_t config;
  message_queue_t message_queue;
  output_sink_list_t file_sinks;
} log_context_t;

static log_context_t log_context;


int log_context_init(log_context_t context[static 1]) {
  context->config.start_time = time(NULL);
  context->config.min_severity = log_trace;
  int msq_q_flag = message_queue_init(
    &(context->message_queue)
  );
  if (msq_q_flag) {
    return msq_q_flag;
  }
  queue_init(
    &(context->file_sinks),
    output_sink_deleter
  );
  return 0;
}

void log_context_destroy(log_context_t* context) {
  message_queue_destroy(
    &(context->message_queue)
  );
  queue_destroy(
    &(context->file_sinks)
  );
}


int log_init(void) {
  int context_init_flag = log_context_init(&log_context);
  if (context_init_flag) {
    return context_init_flag;
  }
  return 0;
}

void log_destroy(void) {
  log_context_destroy(&log_context);
}

void log_set_min_severity(
  enum log_severity severity
) {
  log_context.config.min_severity = severity;
}

void log_add_sink_f(
  FILE output_stream[static 1],
  bool owning,
  sink_printer printer
) {
  output_sink_t* new_sink = output_sink_new(output_stream, owning, printer);
  queue_push(&(log_context.file_sinks), new_sink);
}

int log_push_record(
  log_record_t record[static 1]
) {
  if (log_context.config.min_severity > record->severity) {
    return -1;
  }
  int post_flag = message_queue_post(
    &(log_context.message_queue),
    record
  );
  if (post_flag) {
    return -1;
  }
  return 0;
}


void log_puts(
  enum log_severity severity,
  const char* message
) {
  struct timespec timestamp;
  timespec_get(&timestamp, TIME_UTC);
  log_record_t* new_record = log_record_new_cpy_buf(
    thrd_current(),
    timestamp,
    severity,
    message
  );
  if (new_record == NULL) {
    //TO DO: error message
    return;
  }
  int push_flag = log_push_record(new_record);
  if (push_flag) {
    //TODO: error message
    log_record_free(new_record);
    return;
  }
}

void log_printf(
  enum log_severity severity,
  const char* format,
  ...
) {
  timepoint_t timestamp;
  timespec_get(&timestamp, TIME_UTC);
  va_list arguments;
  va_start(arguments, format);
  char* message = format_to_new_string_v(format, arguments);
  va_end(arguments);
  if (message == NULL) {
    //TODO: error message
    return;
  }
  log_record_t* new_record = log_record_new_cpy_buf(
    thrd_current(),
    timestamp,
    severity,
    message
  );
  if (new_record == NULL) {
    //TO DO: error message
    return;
  }
  int push_flag = log_push_record(new_record);
  if (push_flag) {
    //TODO: error message
    log_record_free(new_record);
    return;
  }

}

void log_process_some_dur(struct timespec duration) {
  struct timespec deadline;
  timespec_get(&deadline, TIME_UTC);
  deadline.tv_sec += duration.tv_sec;
  deadline.tv_nsec += duration.tv_nsec;

  struct timespec current_time;
  timespec_get(&current_time, TIME_UTC);
  while (
    current_time.tv_sec < deadline.tv_sec &&
    current_time.tv_nsec < deadline.tv_nsec
  ) {
    log_record_t* record = message_queue_pop(&(log_context.message_queue));
    if (record == NULL) {
      break;
    }
    int noprint_count = output_sink_list_print(
      &(log_context.file_sinks),
      record
    );
    if (noprint_count) {
      //TO DO: push a debug message
    }
    log_record_free(record);
    timespec_get(&current_time, TIME_UTC);
  }
}

void log_process_all() {
  while(true) {
    log_record_t* record = message_queue_pop(&(log_context.message_queue));
    if (record == NULL) {
      break;
    }
    int noprint_count = output_sink_list_print(
      &(log_context.file_sinks),
      record
    );
    if (noprint_count) {
      //TO DO: push a debug message
    }
    log_record_free(record);
  }
}