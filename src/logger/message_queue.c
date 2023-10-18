#include "message_queue.h"


int message_queue_init(message_queue_t message_queue[static 1]) {
  queue_init(&(message_queue->queue), log_record_deleter);
  int mtx_flag = mtx_init(&(message_queue->lock), mtx_plain);
  if (mtx_flag != thrd_success) {
    queue_destroy(&(message_queue->queue));
    return mtx_flag;
  }
  return 0;
}

void message_queue_destroy(message_queue_t message_queue[static 1]) {
  queue_destroy(&(message_queue->queue));
  mtx_destroy(&(message_queue->lock));
}

int message_queue_post(
  message_queue_t message_queue[static 1],
  log_record_t record[static 1]
) {
  int mtx_flag = mtx_lock(&(message_queue->lock));
  if (mtx_flag != thrd_success) {
    return mtx_flag;
  }
  int push_flag = queue_push(
    &(message_queue->queue),
    record
  );
  mtx_unlock(&(message_queue->lock));
  if (push_flag != 0) {
    return push_flag;
  }
  return 0;
}

log_record_t* message_queue_pop(
  message_queue_t message_queue[static 1]
) {
  mtx_lock(&(message_queue->lock));
  log_record_t* record = queue_pop(&(message_queue->queue));
  mtx_unlock(&(message_queue->lock));
  return record;
}