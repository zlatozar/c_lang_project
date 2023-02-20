#include "data_structs/stack.h"

#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

#define RESIZE_STEP 100

const unsigned int k_initial_size = 50;

struct stack {
  Object_T* storage;
  Object_T* top;      /* Points to next available for push. */
  unsigned size;
};

#define CURRENT_SIZE(stack) ((stack)->top - (stack)->storage)
#define STORAGE(stack) (stack->storage)

/* __________________________________________________________________________ */

Stack_T
Stack_new(unsigned initial_size)
{
  Require(initial_size > 0);

  Stack_T stack;
  NEW(stack);

  stack->storage = ALLOC(initial_size * sizeof(Object_T));

  /* 'sp' points to storage[0] so stack is empty. */
  stack->top = stack->storage;
  stack->size = initial_size;

  return stack;
}

Stack_T
Stack_new_def(void)
{
  return Stack_new(k_initial_size);
}

bool
Stack_is_empty(Stack_T stack)
{
  Require(stack);
  return stack->top == stack->storage;
}

void
Stack_push(Stack_T stack, Object_T data)
{
  Require(stack);

  if (CURRENT_SIZE(stack) == stack->size) {
    Object_T* p_newstack = RESIZE(stack->storage,
                                  (stack->size + RESIZE_STEP) * sizeof(Object_T*));

    stack->storage = p_newstack;
    stack->top = stack->storage + stack->size;

    stack->size += RESIZE_STEP;
  }
  *stack->top = data;
  stack->top++;
}

bool
Stack_pop(Stack_T stack, Object_T* p_data__)
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
Stack_peel(Stack_T stack, Object_T* p_data__)
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

    Object_T stale_out;
    while (Stack_pop(stack, &stale_out)) {
      if (free_data_fn == NULL) {
        FREE(stale_out);

      } else {
        free_data_fn(stale_out);
      }
    }
  }

  FREE(stack->storage);

  FREE(stack);
  stack = NULL;
}

void
Stack_free(Stack_T stack)
{
  Stack_destroy(stack, NULL);
}
