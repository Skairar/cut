#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdatomic.h>
#include <time.h>
#include <threads.h>

#include "cpu_diagnostics/linux.h"

//#include "data_structures/queue.h"
//#include "data_structures/list.h"
#include "logger/logger.h"
#include "data_structures/message_queue.h"

int logger_thread_executor(
  void* should_continue_ptr
) {
  atomic_bool* should_continue = should_continue_ptr;
  struct timespec flush_delay = {
    .tv_sec = 1,
    .tv_nsec = 0
  };
  while (atomic_load(should_continue)) {
    log_process_some_dur(flush_delay);
  }
  log_process_all();
  return 0;
}

int main(void) {
  log_init();
  FILE* log_file = fopen("./log", "w");
    if (log_file == NULL) {
    printf("oh fuck\n");
    return -1;
  }
  log_add_sink_f(log_file, true, NULL);
  log_add_sink_f(stderr, false, NULL);

  atomic_bool execution_flag;
  atomic_init(&execution_flag, true);
  thrd_t worker;
  thrd_create(&worker, logger_thread_executor, &execution_flag);

  FILE* stat = fopen("/proc/stat", "r");
  stat_layout_set_f(stat);
  fclose(stat);
  stat_layout_t layout = stat_layout_get();

  log_puts(log_info, "Stat layout initialized.");

  stat = fopen("/proc/stat", "r");
  stat_cpu_array_t prev = stat_cpu_array_create();
  stat_cpu_array_t curr = stat_cpu_array_create();
  stat_cpu_array_read_f(curr, stat);
  fclose(stat);
  for (size_t i = 0; i < 5; ++i) {
    log_puts(log_trace, "Loop starts.");
    sleep(1);
    stat = fopen("/proc/stat", "r");
    stat_cpu_array_t temp = prev;
    prev = curr;
    curr = temp;
    stat_cpu_array_read_f(curr, stat);
    for (size_t j = 0; j < layout.cpu_count; ++j) {
      stat_cpu_row_delta(prev[j], curr[j], prev[j]);
      printf("[%zu] = %.2f%%, ", j, (double)stat_cpu_row_percentage_8(prev[j]));
    }
    fclose(stat);
    printf("\n");
  }
  stat_cpu_array_free(prev);
  stat_cpu_array_free(curr);
  
  atomic_store(&execution_flag, false);
  thrd_join(worker, NULL);

  log_destroy();
  exit(EXIT_SUCCESS);
}
