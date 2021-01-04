#include <stddef.h>

#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/stack.h"

struct Stack_T {
  int count;

  struct elem {
    void *x;
    struct elem *link;
  } *head;
};

Stack_T Stack_new(void) {
  Stack_T stk;
  NEW(stk);

  stk->count = 0;
  stk->head = NULL;

  return stk;
}

void Stack_free(Stack_T *stk) {
  struct elem *t, *u;

  Assert(stk && *stk);

  for (t = (*stk)->head; t; t = u) {
    u = t->link;
    FREE(t);
  }

  FREE(*stk);
}

int Stack_empty(Stack_T stk) {
  Assert(stk);
  return stk->count == 0;
}

void Stack_push(Stack_T stk, void *x) {
  struct elem *t;
  Assert(stk);
  NEW(t);

  t->x = x;
  t->link = stk->head;

  stk->head = t;
  stk->count++;
}

void *Stack_pop(Stack_T stk) {
  void *x;
  struct elem *t;

  Assert(stk);
  Assert(stk->count > 0);

  t = stk->head;
  stk->head = t->link;
  stk->count--;

  x = t->x;

  FREE(t);
  return x;
}
