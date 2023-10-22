#ifndef SKAI_DATA_STRUCTURES_MESSAGE_QUEUE_H
#define SKAI_DATA_STRUCTURES_MESSAGE_QUEUE_H

#include <threads.h>
#include <time.h>

#include "queue.h"
#include "utilities/time.h"

/**
 * @file Expands on queue to make it work in multithreading environment.
 * All the operations are wrapping around functions provided by regular queue,
 * so one should look into its documentation if more detail is needed.
 */

/**
 * @brief A wrapper for generic queue adapting it to work as means of
 * communication between threads.
 * 
 * Accessing struct fields directly is not recommended.
 */
typedef struct message_queue {
  queue_t queue;    /**<owned queue*/
  mtx_t lock;       /**<lock controlling the access*/
  cnd_t wait;       /**<condition variable for waiting for new messages*/
} message_queue_t;

/**
 * @brief Initializes message queue
 * 
 * @param message_queue Queue to initialize.
 * @param deleter Clean-up function for queue that owns its element or NULL
 * otherwise, although non-owning queues for multithreading are undefined.
 * @return 0 on success, non-0 value on error.
 */
int message_queue_init(
  message_queue_t message_queue[static 1],
  queue_deleter deleter
);

/**
 * @brief Frees the queue
 * 
 * @param message_queue 
 */
void message_queue_destroy(message_queue_t message_queue[static 1]);

/**
 * @brief Pushes the message to the queue, might block.
 * 
 * @param message_queue 
 * @param message 
 * @return 0 on success, non-0 otherwise
 */
int message_queue_push(
  message_queue_t message_queue[static 1],
  void* message
);

/**
 * @brief Pops the element from queue, might block.
 * 
 * @param message_queue 
 * @return Pointer to the element on success, NULL in case of there being no
 * elements of access failure, queue revokes ownership of the returned object
 * and it's up to the caller to perform necessary cleanup on it
 */
void* message_queue_pop(
  message_queue_t message_queue[static 1]
);

/**
 * @brief Blocks until it can pop an element from queue
 * 
 * @param message_queue 
 * @return Pointer to the element or NULL on access failure, caller takes
 * ownership.
 */
void* message_queue_pop_wait(
  message_queue_t message_queue[static 1]
);

/**
 * @brief Blocks until it can pop an element from queue or until a specific
 * timepoint
 * 
 * @param message_queue 
 * @param timepoint
 * @return Pointer to element if succeeded, NULL if no elements were available
 * before the timeout, caller takes ownership.
 */
void* message_queue_pop_wait_t(
  message_queue_t message_queue[static 1],
  timepoint_t timepoint
);



#endif
