#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <threads.h>

#include "utilities/time.h"
#include "cpu_diagnostics/linux.h"
#include "logger/logger.h"
#include "data_structures/message_queue.h"

#include "threads/execution_frame.h"
#include "threads/thread_context.h"
#include "threads/frames/reader.h"
#include "threads/frames/analyzer.h"
#include "threads/frames/printer.h"
#include "threads/frames/logger.h"

static atomic_bool execution_flag;


static void signal_handler(int signum) {
  (void)signum;
  atomic_store(&execution_flag, false);
}

/**
 * @brief Prints percentages of processor usage in the last second every second,
 * also logs information about its behaviour into ./log file.
 * 
 * Can be stopped with sigterm and sigint(ctrl+c in terminal), which will then
 * shutdown the application after a little over a second as it will print all
 * the statistics into ./log. Will also stop in case of any of the threads
 * taking more than 2 seconds to complete a loop iteration.
 * 
 * @return int 
 */
int main(void) {
  
  log_init();
  FILE* log_file = fopen("./log", "w");
  log_add_sink_f(log_file, true, NULL);
  //log_add_sink_f(stderr, false, NULL);

  log_set_min_severity(log_trace);

  FILE* stat = fopen("/proc/stat", "r");
  stat_layout_set_f(stat);
  fclose(stat);

  message_queue_t unprocessed_data_queue;
  message_queue_init(&unprocessed_data_queue, stat_cpu_array_deleter);
  message_queue_t processed_data_queue;
  message_queue_init(&processed_data_queue, stat_cpu_percentage_array_deleter);

  atomic_init(&execution_flag, true);
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  atomic_bool watchdog_flag[4];

  reader_context_t reader_domain = {
    .output = &unprocessed_data_queue
  };
  analyzer_context_t analyzer_domain = {
    .input = &unprocessed_data_queue,
    .output = &processed_data_queue
  };
  printer_context_t printer_domain = {
    .input = &processed_data_queue
  };

  thread_context_t contexts[4] = {
    [0] = {
      .frame = reader_frame,
      .interval = timespan_s_ns(1, 0),
      .name = "Reader",
      .stack_size = 0,
      .domain = &reader_domain,
    },
    [1] = {
      .frame = analyzer_frame,
      .interval = timespan_s_ns(1, 0),
      .name = "Analyzer",
      .stack_size = 0,
      .domain = &analyzer_domain
    },
    [2] = {
      .frame = printer_frame,
      .interval = timespan_s_ns(1, 0),
      .name = "Printer",
      .stack_size = 0,
      .domain = &printer_domain
    },
    [3] = {
      .frame = logger_frame,
      .interval = timespan_s_ns(1, 0),
      .name = "Logger",
      .stack_size = 0,
    }
  };

  thrd_t worker[4];

  for (size_t i = 0; i < 4; ++i) {
    contexts[i].should_continue = &execution_flag;
    contexts[i].watchdog = &(watchdog_flag[i]);
    thrd_create(&(worker[i]), execution_frame, &(contexts[i]));
  }

  timespan_t interval = timespan_s_ns(2, 0);

  size_t flag_id;

  while (atomic_load(&execution_flag)) {
    timepoint_t loop_start = timepoint_now();
    timepoint_t loop_end = timepoint_after(loop_start, interval);
    execution_frame_sleep_until(loop_end);

    for (flag_id = 0; flag_id < 4; ++flag_id) {
      if (!atomic_exchange(&(watchdog_flag)[flag_id], false)) {
        break;
      }
    }
    if (flag_id != 4) {
      log_printf(
        log_fatal,
        "<Watchdog> Thread %zu failed to report back, terminating.",
        flag_id
      );
      atomic_store(&execution_flag, false);
    } 
  }
  
  for (size_t i = 0; i < 4; ++i) {
    thrd_join(worker[i], NULL);
  }

  message_queue_destroy(&(unprocessed_data_queue));
  message_queue_destroy(&(processed_data_queue));
  log_destroy();
  exit(EXIT_SUCCESS);
}
