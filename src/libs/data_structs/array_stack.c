#include "data_structs/stack.h"

#include <stdlib.h>       /* malloc, realloc */
#include "lang/assert.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

#define STACK_INCREMENT 100

#define CURRENT_STACKSIZE(stack) ((stack)->top - (stack)->storage)

/* __________________________________________________________________________ */

mem_status
Stack_init(Stack_T stack)
{
  stack->storage = malloc(STACK_INCREMENT * sizeof(Generic_T));

  if (stack->storage == NULL)
  { return ERROR; }

  /* 'sp' points to storage[0] so stack is empty. */
  stack->top = stack->storage;
  stack->size = STACK_INCREMENT;

  return OK;
}

bool
Stack_is_empty(Stack_T stack)
{
  return stack->top == stack->storage;
}

mem_status
Stack_push(Stack_T stack, Generic_T data)
{
  Require(stack);

  if (CURRENT_STACKSIZE(stack) == stack->size) {
    Generic_T* p_newstack =
      realloc(stack->storage, (stack->size + STACK_INCREMENT) * sizeof(Generic_T*));

    if (p_newstack == NULL)
    { return ERROR; }

    stack->storage = p_newstack;
    stack->top = stack->storage + stack->size;

    stack->size += STACK_INCREMENT;
  }
  *stack->top = data;
  stack->top++;

  return OK;
}

void
Stack_pop(Stack_T stack, Generic_T* p_data__)
{
  Require(stack);

  /* Next push will override poped data. */
  stack->top--;
  *p_data__ = *stack->top;
}

/* It is `pop` then `push` again */
void
Stack_peel(Stack_T stack, Generic_T* p_data__)
{
  Stack_pop(stack, p_data__);
  Stack_push(stack, *p_data__);
}
