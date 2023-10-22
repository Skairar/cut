#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdatomic.h>
#include <time.h>
#include <threads.h>

#include "cpu_diagnostics/linux.h"

#include "logger/logger.h"
#include "data_structures/message_queue.h"


static atomic_bool execution_flag;

void signal_handler(int signum) {
  atomic_store(&execution_flag, false);
}


typedef struct thread_context {
  atomic_bool* should_continue;
  atomic_bool* watchdog;
  timespan_t interval;
} thread_context_t;

typedef struct reader_context {
  thread_context_t thread;
  message_queue_t* output;
} reader_context_t;

typedef struct analyzer_context {
  thread_context_t thread;
  message_queue_t* input;
  message_queue_t* output;
} analyzer_context_t;

typedef struct printer_context {
  thread_context_t thread;
  message_queue_t* input;
} printer_context_t;

int reader_exec(
  void* reader_context_ptr
) {
  reader_context_t* context = reader_context_ptr;
  long long unsigned int iteration_count = 0;
  log_puts(log_trace, "<Reader> Thread starts.");

  while (atomic_load(context->thread.should_continue)) {
    int read_flag = -1;
    int push_flag = -1;

    atomic_store(context->thread.watchdog, true);

    timepoint_t loop_start = timepoint_now();
    timepoint_t loop_end = timepoint_after(loop_start, context->thread.interval);

    FILE* proc_stat = fopen("/proc/stat", "r");
    if (proc_stat == NULL) {
      log_puts(log_fatal, "<Reader> Failed to open /proc/stat.");
      atomic_store(context->thread.should_continue, false);
      break;
    }

    stat_cpu_array_t data = stat_cpu_array_create();
    if (data != NULL) {
      read_flag = stat_cpu_array_read_f(data, proc_stat);
    } else {
      //TO DO: out of memory condition
      fclose(proc_stat);
      continue;
    }

    fclose(proc_stat);

    if (read_flag == 0) {
      push_flag = message_queue_push(context->output, data);
      log_puts(log_trace, "<Reader> Pushed message to queue.");
    } else {
      log_printf(
        log_error,
        "<Reader> Read from /stat/cpu failed, return code: %i.",
        read_flag
      );
    }

    if (push_flag) {
      log_printf(
        log_error, 
        "<Reader> Failed to push results, return code: %i.",
        push_flag
      );
    }

    timespan_t time_remaining = timespan_time_left(
      loop_start,
      loop_end,
      timespan_dur(loop_start, timepoint_now())
    );
    //looping it in case signal wakes it up
    while (timepoint_gt(loop_end, timepoint_now())) {
      log_puts(log_trace, "<Reader> thread sleeps.");
      thrd_sleep(&time_remaining, &time_remaining);
      log_puts(log_trace, "<Reader> thread wakes up.");
    }

    timespan_t total_dur = timespan_dur(loop_start, timepoint_now());
    log_printf(
      log_debug,
      "<Reader> Iteration #%llx finished in %llisec %llinsec.",
      iteration_count,
      (long long int)total_dur.tv_sec,
      (long long int)total_dur.tv_nsec
    );
    iteration_count += 1;
  }
  log_printf(
    log_trace,
    "<Reader> Thread ends after %lli iterations.",
    iteration_count
  );
  return iteration_count;
}

int analyzer_exec(
  void* analyzer_context_ptr
) {
  analyzer_context_t* context = analyzer_context_ptr;
  long long unsigned int iteration_count;
  log_puts(log_trace, "<Analyzer> Thread starts.");

  stat_cpu_array_t prev = NULL;  
  stat_cpu_array_t curr = NULL;
  stat_cpu_array_t delta = stat_cpu_array_create();

  if (delta == NULL) {
    //TO DO: out of memory condition
    atomic_store(context->thread.should_continue, false);
    return -1;
  }

  stat_cpu_percentage_array_t result = NULL;

  while (atomic_load(context->thread.should_continue)) {
    atomic_store(context->thread.watchdog, true);
    timepoint_t loop_start = timepoint_now();
    timepoint_t loop_end = timepoint_after(loop_start, context->thread.interval);

    while (timepoint_gt(loop_end, timepoint_now())) {     
      log_puts(log_trace, "<Analyzer> Attempting to fetch input.");
      curr = message_queue_pop_wait_t(context->input, loop_end);
      if (curr == NULL) {
        log_puts(log_trace, "<Analyzer> Fetch timed out.");
      } else if (prev != NULL) {
        log_puts(log_trace, "<Analyzer> Input fetched, processing.");
        result = stat_cpu_percentage_array_create();
        if (result == NULL) {
          //TO DO: Out of memory
          stat_cpu_array_free(prev);
        } else {
          log_puts(log_trace, "<Analyzer> Calculating results.");
          stat_cpu_array_delta(prev, curr, delta);
          stat_cpu_percentage_array_calculate(result, delta);

          stat_cpu_array_free(prev);
          int push_flag = message_queue_push(context->output, result);
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
        prev = curr;
      } else {
        prev = curr;
      }
    }

    timespan_t total_dur = timespan_dur(loop_start, timepoint_now());
    log_printf(
      log_debug,
      "<Analyzer> Iteration #%llx finished in %llisec %llinsec.",
      iteration_count,
      (long long int)total_dur.tv_sec,
      (long long int)total_dur.tv_nsec
    );
    iteration_count += 1;
  }

  stat_cpu_array_free(delta);

  log_printf(
    log_trace,
    "<Analyzer> Thread ends after %lli iterations.",
    iteration_count
  );

  return 0;
}

int printer_exec(
  void* printer_context_ptr
) {
  printer_context_t* context = printer_context_ptr;
  long long unsigned int iteration_count = 0;
  log_puts(log_trace, "<Printer> Thread starts.");

  stat_cpu_percentage_array_t result = NULL;

  stat_layout_t layout = stat_layout_get();

  while (atomic_load(context->thread.should_continue)) {
    atomic_store(context->thread.watchdog, true);
    timepoint_t loop_start = timepoint_now();
    timepoint_t loop_end = timepoint_after(loop_start, context->thread.interval);

    log_puts(log_trace, "<Printer> Attempting to fetch message.");
    result = message_queue_pop_wait_t(context->input, loop_end);

    if (result == NULL) {
      //means there's no need for sleep
      log_puts(log_trace, "<Printer> Fetch timed out.");
    } else {
      log_puts(log_trace, "<Printer> Message fetched, printing.");
      puts("\nUsage report:");
      for (size_t i = 1; i < layout.cpu_count; ++i) {
        printf("CPU_core[%zu]_usage = %f%%\n", i - 1, result[i]);
      }
      printf("CPU_total = %f%%\n", result[0]);

      stat_cpu_percentage_array_free(result);

      timespan_t time_remaining = timespan_time_left(
        loop_start,
        loop_end,
        timespan_dur(loop_start, timepoint_now())
      );
      //looping it in case signal wakes it up
      while (timepoint_gt(loop_end, timepoint_now())) {
        log_puts(log_trace, "<Printer> Thread sleeps.");
        thrd_sleep(&time_remaining, &time_remaining);
        log_puts(log_trace, "<Printer> Thread wakes up.");
      }
    }

    timespan_t total_dur = timespan_dur(loop_start, timepoint_now());
    log_printf(
      log_debug,
      "<Printer> Iteration #%llx finished in %llisec %llinsec.",
      iteration_count,
      (long long int)total_dur.tv_sec,
      (long long int)total_dur.tv_nsec
    );
    iteration_count += 1;
  }

  log_printf(
    log_trace,
    "<Printer> Thread ends after %lli iterations.",
    iteration_count
  );
  return iteration_count;
}


int logger_exec(
  void* thread_context_ptr
) {
  thread_context_t* context = thread_context_ptr;
  log_puts(log_trace, "<Logger> Thread starts.");

  while (atomic_load(context->should_continue)) {
    atomic_store(context->watchdog, true);
    log_process_some_dur(context->interval);
  }
  log_puts(log_trace, "<Logger> Thread ending, printing remaining messages.");
  log_process_all();
  return 0;
}

int main(void) {
  
  log_init();
  FILE* log_file = fopen("./log", "w");
  log_add_sink_f(log_file, true, NULL);
  //log_add_sink_f(stderr, false, NULL);

  log_set_min_severity(log_debug);

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

  reader_context_t reader_context = {
    .thread = {
      .should_continue = &execution_flag,
      .watchdog = &(watchdog_flag[0]),
      .interval = timespan_s_ns(1, 0)
    },
    .output = &unprocessed_data_queue
  };

  analyzer_context_t analyzer_context = {
    .thread = {
      .should_continue = &execution_flag,
      .watchdog = &(watchdog_flag[1]),
      .interval = timespan_s_ns(1, 0)
    },
    .input = &unprocessed_data_queue,
    .output = &processed_data_queue
  };

  printer_context_t printer_context = {
    .thread = {
      .should_continue = &execution_flag,
      .watchdog = &(watchdog_flag[2]),
      .interval = timespan_s_ns(1, 0)
    },
    .input = &processed_data_queue
  };

  thread_context_t logger_context = {
    .should_continue = &execution_flag,
    .watchdog = &(watchdog_flag[3]),
    .interval = timespan_s_ns(1, 0)
  };

  thrd_t worker[4];

  thrd_create(&(worker[0]), reader_exec, &reader_context);
  thrd_create(&(worker[1]), analyzer_exec, &analyzer_context);
  thrd_create(&(worker[2]), printer_exec, &printer_context);
  thrd_create(&(worker[3]), logger_exec, &logger_context);

  timespan_t interval = timespan_s_ns(2, 0);

  size_t flag_id;

  while (atomic_load(&execution_flag)) {
    timepoint_t loop_start = timepoint_now();
    timepoint_t loop_end = timepoint_after(loop_start, interval);
    timespan_t remaining = interval;
    while (timepoint_gt(loop_end, timepoint_now())) {
      log_puts(log_trace, "<Watchdog> Thread sleeps.");
      thrd_sleep(&remaining, &remaining);
      log_puts(log_trace, "<Watchdog> Thread wakes up.");
    }

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

  log_destroy();
  exit(EXIT_SUCCESS);
}
