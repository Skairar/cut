#ifndef SKAI_LOGGER_OUTPUT_SINKS_H
#define SKAI_LOGGER_OUTPUT_SINKS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data_structures/queue.h"

#include "log_record.h"

typedef int (*sink_printer)(FILE[static 1], log_record_t[static 1]);

typedef struct output_sink {
  FILE* stream;
  bool owning;
  sink_printer printer;
} output_sink_t;

typedef queue_t output_sink_list_t;

output_sink_t* output_sink_new(
  FILE stream[static 1],
  bool owning,
  sink_printer printer
);

void output_sink_free(output_sink_t* output_sink);

void output_sink_deleter(void* output_sink);

int output_sink_default_printer(
  FILE stream[static 1],
  log_record_t record[static 1]
);

int output_sink_list_print(
  output_sink_list_t sink_list[static 1],
  log_record_t record[static 1]
);


#endif
