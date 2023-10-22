#include "log_record.h"

#include <string.h>
#include <stdlib.h>

log_record_t* log_record_new(
  thrd_t thread_id,
  timepoint_t timestamp,
  enum log_severity severity,
  char* message
) {
  log_record_t* new_record = malloc(sizeof(log_record_t));
  if (new_record == NULL) {
    return NULL;
  }
  new_record->thread_id = thread_id;
  new_record->timestamp = timestamp;
  new_record->severity = severity;
  new_record->message = message;
  return new_record;
}

log_record_t* log_record_new_cpy_buf(
  thrd_t thread_id,
  timepoint_t timestamp,
  enum log_severity severity,
  const char* message
) {
  char* buff = malloc(sizeof(char) * (strlen(message) + 1));
  if (buff == NULL) {
    return NULL;
  }
  strcpy(buff, message);
  return log_record_new(
    thread_id,
    timestamp,
    severity,
    buff
  );
}

void log_record_free(log_record_t* record) {
  free(record->message);
  free(record);
}

void log_record_deleter(void* record) {
  log_record_free(record);
}
