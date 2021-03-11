#include "data_structs/stack.h"

#include <stdlib.h>       /* malloc, realloc, NULL */
#include "lang/assert.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

#define STACK_INCREMENT 100

struct stack {
  Generic_T* storage;
  Generic_T* top;      /* Points to next available for push. */
  unsigned size;
} empty_stack = { .storage = NULL,
                  .top = NULL,
                  .size = 0 };

#define CURRENT_STACKSIZE(stack) ((stack)->top - (stack)->storage)

/* __________________________________________________________________________ */

Stack_T
Stack_new(void)
{
  /* Initialize underneath. */
  Stack_T stack = malloc(sizeof(*stack));

  stack->storage = malloc(STACK_INCREMENT * sizeof(Generic_T));

  if (stack->storage == NULL) {
    Log_error("Can't create stack. 'malloc' does not succeed.");
    return NULL;
  }

  /* 'sp' points to storage[0] so stack is empty. */
  stack->top = stack->storage;
  stack->size = STACK_INCREMENT;

  return stack;
}

bool
Stack_is_empty(Stack_T stack)
{
  Require(stack);
  return stack->top == stack->storage;
}

mem_status
Stack_push(Stack_T stack, Generic_T data)
{
  Require(stack);

  if (CURRENT_STACKSIZE(stack) == stack->size) {
    Generic_T* p_newstack =
      realloc(stack->storage, (stack->size + STACK_INCREMENT) * sizeof(Generic_T*));

    if (p_newstack == NULL) {
      Log_error("Root cause - 'realloc' does not succeed.");
      return ERROR;
    }

    stack->storage = p_newstack;
    stack->top = stack->storage + stack->size;

    stack->size += STACK_INCREMENT;
  }
  *stack->top = data;
  stack->top++;

  return OK;
}

status
Stack_pop(Stack_T stack, Generic_T* p_data__)
{
  Require(stack);

  if (Stack_is_empty(stack))
    return FAIL;

  /* Next push will override poped data. */
  stack->top--;
  *p_data__ = *stack->top;
  return SUCC;
}

/* It is `pop` then `push` again. */
status
Stack_peel(Stack_T stack, Generic_T* p_data__)
{
  if (Stack_pop(stack, p_data__) == FAIL)
    return FAIL;

  return Stack_push(stack, *p_data__) == ERROR ? FAIL : SUCC;
}

void
Stack_destroy(Stack_T stack, free_data_FN free_data_fn)
{
  Require(stack);

  if (!Stack_is_empty(stack)) {
    Log_info("Stack is not empty.");

    Generic_T stale_out;
    while (Stack_pop(stack, &stale_out) == SUCC) {
      free_data_fn(stale_out);
    }
  }

  free(stack->storage);
  *stack = empty_stack;

  free(stack);
  stack = NULL;
}
