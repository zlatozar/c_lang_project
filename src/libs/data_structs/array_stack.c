#include "data_structs/stack.h"

#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

#define INITIAL_SIZE 50
#define RESIZE_STEP 100

struct stack {
  Generic_T* storage;
  Generic_T* top;      /* Points to next available for push. */
  unsigned size;
} empty_stack = { .storage = NULL,
                  .top = NULL,
                  .size = 0
                };

#define CURRENT_SIZE(stack) ((stack)->top - (stack)->storage)
#define STORAGE(stack) (stack->storage)

/* __________________________________________________________________________ */

Stack_T
Stack_new(void)
{
  Stack_T stack;
  NEW(stack);

  stack->storage = ALLOC(INITIAL_SIZE * sizeof(Generic_T));

  /* 'sp' points to storage[0] so stack is empty. */
  stack->top = stack->storage;
  stack->size = INITIAL_SIZE;

  return stack;
}

bool
Stack_is_empty(Stack_T stack)
{
  Require(stack);
  return stack->top == stack->storage;
}

void
Stack_push(Stack_T stack, Generic_T data)
{
  Require(stack);

  if (CURRENT_SIZE(stack) == stack->size) {
    Generic_T* p_newstack = RESIZE(stack->storage,
                                   (stack->size + RESIZE_STEP) * sizeof(Generic_T*));

    stack->storage = p_newstack;
    stack->top = stack->storage + stack->size;

    stack->size += RESIZE_STEP;
  }
  *stack->top = data;
  stack->top++;
}

bool
Stack_pop(Stack_T stack, Generic_T* p_data__)
{
  Require(stack);

  if (Stack_is_empty(stack)) {
    return false;
  }

  /* Next push will override poped data. */
  stack->top--;
  *p_data__ = *stack->top;
  return true;
}

/* Note: It is `pop` then `push` operation. */
bool
Stack_peel(Stack_T stack, Generic_T* p_data__)
{
  if (!Stack_pop(stack, p_data__))
  { return false; }

  Stack_push(stack, *p_data__);
  return true;
}

void
Stack_destroy(Stack_T stack, free_data_FN free_data_fn)
{
  if (!Stack_is_empty(stack)) {
    Log_warn("Stack is not empty.");

    Generic_T stale_out;
    while (Stack_pop(stack, &stale_out)) {
      if (free_data_fn == NULL) {
        FREE(stale_out);

      } else {
        free_data_fn(stale_out);
      }
    }
  }

  FREE(stack->storage);
  *stack = empty_stack;

  FREE(stack);
  stack = NULL;
}

void
Stack_free(Stack_T stack)
{
  Stack_destroy(stack, NULL);
}
