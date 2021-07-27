/**
 * @file    queue.h
 * @brief   Queue ADT interface.
 */
#if !defined(DATA_STRUCTS_QUEUE_H)
#define DATA_STRUCTS_QUEUE_H

#include <stddef.h>     /* size_t */
#include "lang/extend.h"

typedef struct queue*  Queue_T;

/**
 * Initialize a list by setting the list pointer to NULL.
 */
extern Queue_T Queue_new(void);

/**
 * Return `true` if given queue is an empty queue, `false` otherwise.
 */
extern bool Queue_is_empty(Queue_T queue);

/**
 * Add given data to the queue.
 */
extern void Queue_add(Queue_T queue, Object_T data);

/**
 * Remove a value from queue and put in `p_data`.
 */
bool Queue_remove(Queue_T queue, Object_T* p_data);

/**
 * @brief    Number of list elements.
 */
extern size_t Queue_length(Queue_T queue);

/**
 * @brief    Delete every node in the given queue.
 *
 * Use it if data stored in list needs special way of freeing
 * otherwise use `Queue_free`.
 */
extern void Queue_destroy(Queue_T queue, free_data_FN free_data_fn);

/**
 * @brief    Frees occupied memory.
 */
extern void Queue_free(Queue_T queue);

#endif  /* DATA_STRUCTS_QUEUE_H */
