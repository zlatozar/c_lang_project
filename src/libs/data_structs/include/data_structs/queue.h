/**
 * @file    queue.h
 * @brief   Queue ADT interface.
 */
#if !defined(DATA_STRUCTS_QUEUE_H)
#define DATA_STRUCTS_QUEUE_H

#include <stdbool.h>    /* bool   */
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
extern void Queue_add(Queue_T queue, Generic_T data);

#endif  /* DATA_STRUCTS_QUEUE_H */
