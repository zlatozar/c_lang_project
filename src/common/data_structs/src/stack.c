#include <stddef.h>  /* NULL */
#include "lang/assert.h"
#include "lang/mem.h"

#include "data_structs/stack.h"

struct stack {
  int count;
  struct elem {
    void* x;
    struct elem* link;
  }* sp;
};

Stack_T
Stack_new(void)
{
  Stack_T self;
  NEW(self);

  self->count = 0;
  self->sp = NULL;

  return self;
}

void
Stack_free(Stack_T* self)
{
  struct elem* t, *u;

  Assert(self && *self);

  for (t = (*self)->sp; t; t = u) {
    u = t->link;
    FREE(t);
  }

  FREE(*self);
}

bool
Stack_isEmpty(Stack_T self)
{
  Assert(self);
  return self->count == 0;
}

void
Stack_push(Stack_T self, void* x)
{
  struct elem* t;

  Assert(self);
  NEW(t);

  t->x = x;
  t->link = self->sp;

  self->sp = t;
  self->count++;
}

void*
Stack_pop(Stack_T self)
{
  void* x;
  struct elem* t;

  Assert(self);
  Assert(self->count > 0);

  t = self->sp;
  self->sp = t->link;
  self->count--;

  x = t->x;

  FREE(t);

  return x;
}
