#ifndef SKAI_LOGGER_MESSAGE_QUEUE_H
#define SKAI_LOGGER_MESSAGE_QUEUE_H

#include <threads.h>

#include "data_structures/queue.h"

#include "log_record.h"

typedef struct message_queue {
  queue_t queue;
  mtx_t lock;
} message_queue_t;

int message_queue_init(message_queue_t message_queue[static 1]);

void message_queue_destroy(message_queue_t message_queue[static 1]);

int message_queue_post(
  message_queue_t message_queue[static 1],
  log_record_t record[static 1]
);

log_record_t* message_queue_pop(
  message_queue_t message_queue[static 1]
);

#endif