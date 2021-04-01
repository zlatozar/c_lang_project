#include "data_structs/lstack.h"
#include "data_structs/list.h"

#include "lang/memory.h"
#include "lang/assert.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

struct stack {
  int count;
  List_T storage;
};

/* __________________________________________________________________________ */

LStack_T
LStack_new(void)
{
  LStack_T stack;
  NEW(stack);

  stack->count = 0;
  stack->storage = List_new();

  return stack;
}

bool
LStack_is_empty(LStack_T stack)
{
  Require(stack);
  return stack->count == 0;
}

void
LStack_push(LStack_T stack, Generic_T data)
{
  Require(stack);

  List_insert(&stack->storage, data);
  stack->count++;
}

bool
LStack_pop(LStack_T stack, Generic_T* p_data__)
{
  Require(stack);

  if (stack->count == 0) {
    return false;
  }

  List_delete_head(&stack->storage, p_data__);
  stack->count--;
  return true;
}

bool
LStack_peel(LStack_T stack, Generic_T* p_data__)
{
  if (!LStack_pop(stack, p_data__))
  { return false; }

  LStack_push(stack, *p_data__);
  return true;
}

void
LStack_destroy(LStack_T stack, free_data_FN free_data_fn)
{
  if (!LStack_is_empty(stack)) {
    Log_warn("LStack is not empty.");

    List_destroy(&stack->storage, free_data_fn);
    stack->count = 0;
  }

  FREE(stack);
  stack = NULL;
}

void
LStack_free(LStack_T stack)
{
  LStack_destroy(stack, NULL);
}
