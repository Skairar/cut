#include "message_queue.h"

int message_queue_init(
  message_queue_t message_queue[static 1],
  queue_deleter deleter
) {
  int mtx_flag = mtx_init(&(message_queue->lock), mtx_plain);
  if (mtx_flag != thrd_success) {
    return mtx_flag;
  }
  int cnd_flag = cnd_init(&(message_queue->wait));
  if (cnd_flag != thrd_success) {
    mtx_destroy(&(message_queue->lock));
    return cnd_flag;
  }
  queue_init(&(message_queue->queue), deleter);
  return 0;
}

void message_queue_destroy(message_queue_t message_queue[static 1]) {
  queue_destroy(&(message_queue->queue));
  cnd_destroy(&(message_queue->wait));
  mtx_destroy(&(message_queue->lock));
}

int message_queue_push(
  message_queue_t message_queue[static 1],
  void* message
) {
  int mtx_flag = mtx_lock(&(message_queue->lock));
  if (mtx_flag != thrd_success) {
    return mtx_flag;
  }
  int push_flag = queue_push(
    &(message_queue->queue),
    message
  );
  cnd_signal(&(message_queue->wait));
  mtx_unlock(&(message_queue->lock));
  if (push_flag != 0) {
    return push_flag;
  }
  return 0;
}

void* message_queue_pop(
  message_queue_t message_queue[static 1]
) {
  int mtx_flag = mtx_lock(&(message_queue->lock));
  if (mtx_flag != thrd_success) {
    //TO DO: figure out whether it's possible in circumstances other than
    //complete heap overflow and entire execution being FUBAR and then maybe
    //think of some better solution
    return NULL;
  }
  void* message = queue_pop(&(message_queue->queue));
  mtx_unlock(&(message_queue->lock));
  return message;
}

void* message_queue_pop_wait(
  message_queue_t message_queue[static 1]
) {
  int mtx_flag = mtx_lock(&(message_queue->lock));
  if (mtx_flag != thrd_success) {
    return NULL;
  }
  void* message = queue_pop(&(message_queue->queue));
  while (message == NULL) {
    //supposedly this function might return thrd_error, but I couldn't
    //find anywhere what has to happen for that + what's the mutex state
    //afterwards, so I'm not implementing handling this for now as I'm
    //not even sure what to do in such situation
    cnd_wait(
      &(message_queue->wait),
      &(message_queue->lock)
    );
    message = queue_pop(&(message_queue->queue));
  }
  mtx_unlock(&(message_queue->lock));
  return message;  
}

void* message_queue_pop_wait_t(
  message_queue_t message_queue[static 1],
  timepoint_t timepoint
) {
  int mtx_flag = mtx_lock(&(message_queue->lock));
  if (mtx_flag != thrd_success) {
    return NULL;
  }
  void* message = queue_pop(&(message_queue->queue));
  while (message == NULL) {
    int cnd_flag = cnd_timedwait(
      &(message_queue->wait),
      &(message_queue->lock),
      &timepoint
    );
    //will return message as NULL in case of timeout
    if (cnd_flag == thrd_timedout) {
      break;
    }
    //in case of spurious wake-up queue_pop will simply return NULL, so
    //there's no need for any additional checks
    message = queue_pop(&(message_queue->queue));
  }
  mtx_unlock(&(message_queue->lock));
  return message;  
}