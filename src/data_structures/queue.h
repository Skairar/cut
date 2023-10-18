#ifndef SKAI_QUEUE_H_
#define SKAI_QUEUE_H_

#include <stdlib.h>
#include <stdbool.h>

typedef struct queue_node {
  void* value;
  //struct queue_node* prev;
  struct queue_node* next;
} queue_node_t;

typedef void (*queue_deleter)(void*);

typedef struct queue {
  queue_node_t* front;
  queue_node_t* back;
  queue_deleter deleter;
} queue_t;

queue_node_t* queue_node_new(void* value);

void queue_node_free(queue_node_t* node);

void queue_init(
  queue_t queue[static 1],
  queue_deleter deleter
);

void queue_destroy(queue_t queue[static 1]);

size_t queue_size(queue_t queue[static 1]);

bool queue_empty(queue_t queue[static 1]);

int queue_push(queue_t queue[static 1], void* value);

void* queue_pop(queue_t queue[static 1]);





#endif
