#include "data_structs/queue.h"

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

/* static void */
/* free_node(node_t** pp_node) */
/* { */
/*   FREE(*pp_node); */
/* } */

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
  return FRONT(queue) == NULL;
}

void
Queue_add(Queue_T queue, Generic_T data)
{
   node_t* new_node;
   allocate_node(&new_node, data);

   if (Queue_is_empty(queue)) {
     FRONT(queue) = REAR(queue) = new_node;

   } else {
     NEXT(REAR(queue)) = new_node;
     REAR(queue) = new_node;
   }
}
