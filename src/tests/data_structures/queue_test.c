#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>


#include "data_structures/queue.h"

static int deleter_invocation_count;

static void sample_deleter(void* value) {
  free(value);
  deleter_invocation_count += 1;
}


int main(void) {

  queue_t sample_queue;

  //Tests on simple type without ownership
  int elements[] = {
    [0] = 35,
    [1] = 46,
    [2] = -242536,
    [3] = 0,
    [4] = -2,
    [5] = 67542,
    [6] = 231,
    [7] = -1341,
    [8] = -13,
    [9] = 0
  };
  queue_init(&sample_queue, NULL);
  assert(queue_empty(&sample_queue) && "Queue is initialized empty.");
  assert((queue_size(&sample_queue) == 0) && "Size of new queue is 0.");
  assert(
    (queue_pop(&sample_queue) == NULL) &&
    "Empty queue return NULL on pop."
  );
  for (size_t i = 0; i < 10; ++i) {
    assert(
      (queue_push(&sample_queue, &(elements[i])) == 0) &&
      "Queue push returns 0 on success."
    );
    assert(
      (queue_size(&sample_queue) == (i + 1)) &&
      "Queue size increases witch each addition"
    );
  }
  assert(
    (!queue_empty(&sample_queue)) &&
    "Queue shouldn't be empty after adding elements."
  );
  for (size_t i = 0; i < 10; ++i) {
    int popped_element = *(int*)queue_pop(&sample_queue);
    assert(
      (popped_element == elements[i]) &&
      "Queue pops elements in the same order as they were pushed."
    );
    assert(
      (queue_size(&sample_queue) == 10 - i - 1) &&
      "Queue size decreases with popping."
    );
  }
  assert(
    (queue_empty(&sample_queue)) &&
    "Queue should be empty now."
  );
  assert(
    (queue_pop(&sample_queue) == NULL) &&
    "Emptied queue returns NULL on pop."
  );
  queue_destroy(&sample_queue);

  //fill the queue again and then destroy to see if destroying a full queue
  //doesn't give a segfault
  queue_init(&sample_queue, NULL);
  for (size_t i = 0; i < 10; ++i) {
    queue_push(&sample_queue, &(elements[i]));
  }
  queue_destroy(&sample_queue);

  //tests on dynamic elements, with queue taking ownership
  queue_init(&sample_queue, sample_deleter);
  for (size_t i = 0; i < 10; ++i) {
    //assuming here malloc won't return NULL
    int *dynamic_element = malloc(sizeof(int));
    *dynamic_element = elements[i];
    assert(
      (queue_push(&sample_queue, dynamic_element) == 0) &&
      "Queue push returns 0 on success."
    );
  }
  assert(
    (queue_size(&sample_queue) == 10) &&
    "There should be 10 elements in queue now"
  );
  assert(
    (queue_push(&sample_queue, NULL) == -1) &&
    "Queue declines pushing a null pointer and returns -1"
  );

  deleter_invocation_count = 0;
  queue_destroy(&sample_queue);
  assert(
    (deleter_invocation_count == 10) &&
    "There were 10 elements and deleter was invoked for all of them."
  );

  return 0;
}
