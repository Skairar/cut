#include "linux.h"

/**
 * @brief File scope variable storing layout for use globally by implementation.
 */
stat_layout_t global_layout;

int stat_layout_set_lf(
  stat_layout_t layout[static 1],
  FILE source[static 1]
) {
  //reset layout to 0 just in case
  layout->cpu_count = 0;
  layout->cpu_column_count = 0;

  //file iterator outside the loop
  int read_character = fgetc(source);

  //count the columns
  while (
    read_character != EOF &&
    read_character != '\n'
  ) {
    if (read_character == ' ') {
      layout->cpu_column_count += 1;
    }
    //make sure to eat duplicate spaces
    do {
      read_character = fgetc(source);
    } while (read_character == ' ');
    //eat all the alphanumerics
    while (isalnum(read_character)) {
      read_character = fgetc(source);
    }
  }

  //should stop reading when it gets to 'intr', the section after cpu stats
  while (read_character != EOF && read_character != 'i') {
    if (read_character == '\n') {
      layout->cpu_count += 1;
    }
    read_character = fgetc(source);
  }
  
  if (read_character == EOF) {
    //function doesn't own the stream, so checking what exactly happened should
    //be up to the caller
    return -1;
  }

  return 0;
}

int stat_layout_set_f(
  FILE source[static 1]
) {
  return stat_layout_set_lf(&global_layout, source);
}

const stat_layout_t stat_layout_get() {
  return global_layout;
}


stat_cpu_array_t stat_cpu_array_create_l(stat_layout_t layout[static 1]) {
  stat_cpu_array_t new_array = malloc(
    sizeof(stat_cpu_row_t) * layout->cpu_count
  );
  for (size_t i = 0; i < layout->cpu_count; ++i) {
    new_array[i] = malloc(sizeof(stat_cpu_field_t) * layout->cpu_column_count);
  }
  return new_array;
}

stat_cpu_array_t stat_cpu_array_create() {
  stat_cpu_array_create_l(&global_layout);
}

void stat_cpu_array_free_l(
  stat_cpu_array_t array,
  stat_layout_t layout[static 1]
) {
  for (size_t i = 0; i < layout->cpu_count; ++i) {
    free(array[i]);
  }
  free(array);
}

void stat_cpu_array_free(stat_cpu_array_t array) {
  stat_cpu_array_free_l(array, &global_layout);
}

int stat_cpu_array_read_fl(
  stat_cpu_array_t array,
  FILE source[static 1],
  stat_layout_t layout[static 1]
) {

  //to eat useless data
  char dummy[10];
  //fscanf returns number of succesful reads
  int err_flag = 1; //1 is the number on success for fscanf in this case
  int read_total = 0; //for counting how many numbers were successfully read

  for (int i = 0; i < layout->cpu_count && err_flag == 1; ++i) {
    //even if it's an unnecessary data, failure here means something's wrong
    err_flag = fscanf(source, "%s", dummy);
    for (int j = 0; j < layout->cpu_column_count && err_flag == 1; ++j) {
      err_flag = fscanf(source, "%lf", &(array[i][j]));
      //necessary because fscanf can also return EOF
      read_total += (err_flag == 1);
    }
  }

  if (err_flag == EOF) {
    return EOF;
  }

  if (err_flag == 0) {
    return read_total;
  }

  return 0; 
}

int stat_cpu_array_read_f(
  stat_cpu_array_t array,
  FILE source[static 1]
) {
  stat_cpu_array_read_fl(array, source, &global_layout);
}