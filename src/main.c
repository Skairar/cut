#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "cpu_diagnostics/linux.h"
//#include "data_structures/queue.h"


int main(void) {
  FILE* stat = fopen("/proc/stat", "r");
  stat_layout_set_f(stat);
  stat_layout_t layout = stat_layout_get();
  stat_cpu_array_t array = stat_cpu_array_create();
  for (size_t i = 0; i < 5; ++i) {
    FILE* stat = fopen("/proc/stat", "r");
    int flag = stat_cpu_array_read_f(array, stat);
    if (flag) {
      printf("Read failure, returned: %d", flag);
      continue;
    }
    printf("Read #%zu\n", i);
    for (size_t j = 0; j < layout.cpu_count; ++j) {
      printf("[%zu] = {", j);
      for (size_t k = 0; k < layout.cpu_column_count; ++k) {
        printf(" %lf", array[j][k]);
      }
      puts(" }\n");
    }
    putc('\n', stdout);
    fclose(stat);
    sleep(1);
  }
  stat_cpu_array_free(array);
  
  exit(EXIT_SUCCESS);
}
