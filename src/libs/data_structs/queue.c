#include "data_structs/queue.h"

#include "lang/assert.h"
#include "lang/memory.h"

/* ______________________________________________________________________________ */
/*                                                                        Locals  */

typedef struct node {
  Generic_T datapointer;
  struct node* next;
} node_t;

#define DATA(p_node) ((p_node)->datapointer)
#define NEXT(p_node) ((p_node)->next)

struct queue {
  node_t* front;
  node_t* rear;
};

#define FRONT(p_queue) ((p_queue)->front)
#define REAR(p_queue)  ((p_queue)->rear)

static void
allocate_node(node_t** pp_node, Generic_T data)
{
  node_t* p_node;
  NEW(p_node);

  *pp_node = p_node;

  p_node->datapointer = data;
  p_node->next = NULL;
}

static node_t*
iterator(node_t* p_node, node_t* last_return)
{
  return (last_return == NULL) ? p_node : NEXT(last_return);
}

static void
free_node(node_t** pp_node)
{
  FREE(*pp_node);
}

/* ______________________________________________________________________________ */

Queue_T
Queue_new(void)
{
  Queue_T queue;
  NEW(queue);

  FRONT(queue) = NULL;
  REAR(queue) = NULL;

  return queue;
}

bool
Queue_is_empty(Queue_T queue)
{
  Require(queue);
  return FRONT(queue) == NULL;
}

void
Queue_add(Queue_T queue, Generic_T data)
{
  Require(queue);

  node_t* new_node;
  allocate_node(&new_node, data);

  if (Queue_is_empty(queue)) {
    FRONT(queue) = REAR(queue) = new_node;

  } else {
    NEXT(REAR(queue)) = new_node;
    REAR(queue) = new_node;
  }
}

bool
Queue_remove(Queue_T queue, Generic_T* p_data)
{
  Require(queue);

  if (Queue_is_empty(queue))
  { return false; }

  node_t* front_node = FRONT(queue);
  *p_data = DATA(front_node);

  if (REAR(queue) == FRONT(queue)) {
    REAR(queue) = FRONT(queue) = NULL;

  } else {
    FRONT(queue) = NEXT(front_node);
  }

  free_node(&front_node);
  return true;
}

size_t
Queue_length(Queue_T queue)
{
  size_t accum = 0;
  node_t* curr_node = NULL;

  while ((curr_node = iterator(FRONT(queue), curr_node)) != NULL)
  { ++accum; }

  return accum;
}

/*
 * Recursively delete all connections then free each node's data if free
 * function is passed.
 */
void
Queue_destroy(Queue_T queue, free_data_FN free_data_fn)
{
  Require(queue);

  if (Queue_is_empty(queue)) {
    return;
  }

  Generic_T stale_out;
  while (Queue_remove(queue, &stale_out)) {
    if (free_data_fn == NULL) {
      FREE(stale_out);

    } else {
      free_data_fn(stale_out);
    }
  }

  FREE(queue);
  queue = NULL;
}

void
Queue_free(Queue_T queue)
{
  Queue_destroy(queue, NULL);
}
