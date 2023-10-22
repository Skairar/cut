#include "queue.h"

queue_node_t* queue_node_new(void* value) {
  if (value == NULL) {
    return NULL;
  }
  queue_node_t* new_node = malloc(sizeof(queue_node_t));
  if (new_node == NULL) {
    return NULL;
  }
  new_node->value = value;
  //new_node->prev = NULL;
  new_node->next = NULL;
  return new_node;
}

void queue_node_free(queue_node_t* node) {
  free(node);
}

void queue_init(
  queue_t queue[static 1],
  queue_deleter deleter
) {
  queue->front = NULL;
  queue->back = NULL;
  queue->deleter = deleter;
}

void queue_destroy(queue_t queue[static 1]) {
  queue_node_t* i = queue->front;
  //chosen loops inside branch instead of branching inside loop
  if (queue->deleter) {
    while (i != NULL) {
      queue_node_t* j = i->next;
      queue->deleter(i->value);
      queue_node_free(i);
      i = j;
    }
  } else {
    while (i != NULL) {
      queue_node_t* j = i->next;
      queue_node_free(i);
      i = j;
    }
  }
  queue->front = NULL;
  queue->back = NULL;
}

size_t queue_size(queue_t queue[static 1]) {
  size_t size = 0;
  for (queue_node_t* i = queue->front; i != NULL; i = i->next) {
    size += 1;
  }
  return size;
}

bool queue_empty(queue_t queue[static 1]) {
  if (queue->back == NULL) {
    return true;
  } else {
    return false;
  }
}

int queue_push(queue_t queue[static 1], void* value) {
  queue_node_t* new_node = queue_node_new(value);
  if (new_node == NULL) {
    return -1;
  }
  if (queue->back == NULL) {
    queue->front = new_node;
    queue->back = new_node;
  } else {
    //new_node->prev = queue->back;
    queue->back->next = new_node;
    queue->back = new_node;
  }
  return 0;
}

void* queue_pop(queue_t queue[static 1]) {
  queue_node_t* node = queue->front;
  if (node == NULL) {
    return NULL;
  }
  if (node->next == NULL) {
    queue->front = NULL;
    queue->back = NULL;
  } else {
    queue->front = node->next;
    //queue->front->prev = NULL;
  }
  void* value = node->value;
  queue_node_free(node);
  return value;
}
