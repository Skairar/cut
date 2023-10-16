#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "cpu_diagnostics/linux.h"
//#include "data_structures/queue.h"
//#include "data_structures/list.h"

//#include "logger/logger.h"

int main(void) {
  FILE* stat = fopen("/proc/stat", "r");
  stat_layout_set_f(stat);
  fclose(stat);
  stat_layout_t layout = stat_layout_get();

  stat = fopen("/proc/stat", "r");
  stat_cpu_array_t prev = stat_cpu_array_create();
  stat_cpu_array_t curr = stat_cpu_array_create();
  stat_cpu_array_read_f(curr, stat);
  fclose(stat);
  for (size_t i = 0; i < 5; ++i) {
    sleep(1);
    stat = fopen("/proc/stat", "r");
    stat_cpu_array_t temp = prev;
    prev = curr;
    curr = temp;
    stat_cpu_array_read_f(curr, stat);
    for (size_t j = 0; j < layout.cpu_count; ++j) {
      stat_cpu_row_delta(prev[j], curr[j], prev[j]);
      printf("Percentage [%zu] = %.2f%%\n", j, stat_cpu_row_percentage_8(prev[j]));
    }
    fclose(stat);
  }
  stat_cpu_array_free(prev);
  stat_cpu_array_free(curr);
  
  exit(EXIT_SUCCESS);
}
