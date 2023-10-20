#ifndef SKAI_QUEUE_H_
#define SKAI_QUEUE_H_

#include <stdlib.h>
#include <stdbool.h>

/**
 * @file Generic queue, storing its values as void pointers. Can be initialized
 * to take ownership of its values and invoke proper cleaning functions in
 * case of destruction.
 */


/**
 * @brief Single node of a generic queue
 * 
 * Can be used for manual iteration if necessary
 */
typedef struct queue_node {
  void* value;               /**<pointer to value*/
  //struct queue_node* prev;
  struct queue_node* next;   /**<pointer to next node*/
} queue_node_t;

/**
 * @brief Alias representing a signature of the function used for cleanup of
 * owned objects
 */
typedef void (*queue_deleter)(void*);

/**
 * @brief Proper queue object
 * 
 * Both front and back are equal when queue has one element:
 * front == back == element_node;
 * or when there are no elements:
 * front == back == NULL;
 * 
 * Deleter equal to NULL denotes a non-owning queue.
 */
typedef struct queue {
  queue_node_t* front;    /**<Pointer to first node of the queue*/
  queue_node_t* back;     /**<Pointer to last node of the queue*/
  queue_deleter deleter;  /**<Pointer to cleanup function*/
} queue_t;

/**
 * @brief Creates new node
 * 
 * @param value Address of element to be stored in new node, which is not a
 * NULL pointer.
 * @return Pointer to new node in case of success, NULL if malloc failed or
 * when NULL pointer was provided as value
 */
queue_node_t* queue_node_new(void* value);

/**
 * @brief Frees memory allocated for the node itself.
 * 
 * @param node Node to be freed.
 */
void queue_node_free(queue_node_t* node);

/**
 * @brief Initializes queue_t object.
 * 
 * @param queue Queue_t object to be initialized.
 * @param deleter Cleanup function for use in case of passing ownership of
 * objects to the queue, or NULL for non-owning queue.
 */
void queue_init(
  queue_t queue[static 1],
  queue_deleter deleter
);

/**
 * @brief Performs a cleanup on the queue, freeing all its nodes and invoking
 * a cleanup function(if such was provided on init) on all leftover values.
 * 
 * @param queue Queue to be freed.
 */
void queue_destroy(queue_t queue[static 1]);

/**
 * @param queue 
 * @return Number of elements in the queue.
 */
size_t queue_size(queue_t queue[static 1]);

/**
 * @param queue 
 * @return true if queue has at least one element, false otherwise
 */
bool queue_empty(queue_t queue[static 1]);

/**
 * @brief Pushes element to the queue.
 * 
 * @param queue Takes ownership if deleter was provided on initialization
 * @param value Pointer to the object pushed to the queue, can't be a NULL
 * pointer
 * @return 0 on success, -1 otherwise
 */
int queue_push(queue_t queue[static 1], void* value);

/**
 * @brief Pops element from the queue.
 * 
 * Revokes ownership of the popped element(in case of owning queue), so it's
 * up to the caller to perform proper cleanup afterwards.
 * 
 * @param queue 
 * @return void* Pointer to the element, NULL if queue is empty.
 */
void* queue_pop(queue_t queue[static 1]);





#endif
