#include "output_sinks.h"

output_sink_t* output_sink_new(
  FILE stream[static 1],
  bool owning,
  sink_printer printer
) {
  output_sink_t* new_sink = malloc(sizeof(output_sink_t));
  new_sink->stream = stream;
  new_sink->owning = owning;
  if (printer) {
    new_sink->printer = printer;
  } else {
    new_sink->printer = output_sink_default_printer;
  }
  return new_sink;
}

void output_sink_free(output_sink_t* output_sink) {
  if (output_sink->owning) {
    fclose(output_sink->stream);
  }
  free(output_sink);
}

void output_sink_deleter(void* output_sink) {
  output_sink_free(output_sink);
}

int output_sink_default_printer(
  FILE stream[static 1],
  log_record_t record[static 1]
) {
  return fprintf(
    stream,
    "(%lld.%.9ld)[%02x](%s): %s\n",
    (long long int)record->timestamp.tv_sec,
    record->timestamp.tv_nsec,
    (unsigned int)record->thread_id,
    log_severity_str(record->severity),
    record->message
  );
}

int output_sink_list_print(
  output_sink_list_t sink_list[static 1],
  log_record_t record[static 1]
) {
  queue_node_t* iter = sink_list->front;
  int unprinted_count = 0;
  while (iter != NULL) {
    output_sink_t* sink = iter->value;
    if (sink->printer(sink->stream, record) < 0) {
      unprinted_count += 1;
    }
    iter = iter->next;
  }
  return unprinted_count;
}