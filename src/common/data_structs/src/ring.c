#include <stdarg.h>  /* va_list, va_start */
#include <stddef.h>  /* NULL              */

#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/ring.h"

struct _Ring_T {
  struct node {
    struct node *llink, *rlink;
    void* value;
  } *head;
  int length;
};

Ring_T Ring_new()
{
  Ring_T ring;
  NEW_0(ring);

  ring->head = NULL;

  return ring;
}

Ring_T Ring_ring(void* x, ...)
{
  va_list ap;
  Ring_T ring = Ring_new();

  va_start(ap, x);

  for ( ; x; x = va_arg(ap, void *))
    Ring_addhi(ring, x);

  va_end(ap);

  return ring;
}

void Ring_free(Ring_T* ring)
{
  struct node *p, *q;

  Assert(ring && *ring);

  if ((p = (*ring)->head) != NULL) {
    int n = (*ring)->length;

    for ( ; n-- > 0; p = q) {
      q = p->rlink;
      FREE(p);
    }
  }

  FREE(*ring);
}

int Ring_length(Ring_T ring)
{
  Assert(ring);
  return ring->length;
}

void* Ring_get(Ring_T ring, int i)
{
  struct node *q;

  Assert(ring);
  Assert(i >= 0 && i < ring->length);
  {
    int n;
    q = ring->head;

    if (i <= ring->length/2) {
      for (n = i; n-- > 0; )
        q = q->rlink;

    } else {
      for (n = ring->length - i; n-- > 0; )
        q = q->llink;
    }
  }

  return q->value;
}

void* Ring_put(Ring_T ring, int i, void* x)
{
  struct node *q;
  void* prev;

  Assert(ring);
  Assert(i >= 0 && i < ring->length);
  {
    int n;
    q = ring->head;

    if (i <= ring->length/2) {
      for (n = i; n-- > 0; )
        q = q->rlink;

    } else {
      for (n = ring->length - i; n-- > 0; )
        q = q->llink;
    }
  }

  prev = q->value;
  q->value = x;

  return prev;
}

void* Ring_add(Ring_T ring, int pos, void* x)
{
  Assert(ring);
  Assert(pos >= -ring->length && pos<=ring->length+1);

  if (pos == 1 || pos == -ring->length) {
    return Ring_addlo(ring, x);

  } else if (pos == 0 || pos == ring->length + 1) {
    return Ring_addhi(ring, x);

  } else {
    struct node *p, *q;

    int i = pos < 0 ? pos + ring->length : pos - 1;
    {
      int n;
      q = ring->head;

      if (i <= ring->length/2) {
        for (n = i; n-- > 0; )
          q = q->rlink;

      } else {
        for (n = ring->length - i; n-- > 0; )
          q = q->llink;
      }
    }

    NEW(p);

    {
      p->llink = q->llink;
      q->llink->rlink = p;

      p->rlink = q;
      q->llink = p;
    }

    ring->length++;

    return p->value = x;
  }
}

void* Ring_addhi(Ring_T ring, void* x)
{
  struct node *p, *q;

  Assert(ring);

  NEW(p);

  if ((q = ring->head) != NULL) {
      p->llink = q->llink;
      q->llink->rlink = p;
      p->rlink = q;
      q->llink = p;

  } else {
    ring->head = p->llink = p->rlink = p;
  }

  ring->length++;

  return p->value = x;
}

void* Ring_addlo(Ring_T ring, void* x)
{
  Assert(ring);

  Ring_addhi(ring, x);
  ring->head = ring->head->llink;

  return x;
}

void* Ring_remove(Ring_T ring, int i)
{
  void* x;
  struct node *q;

  Assert(ring);
  Assert(ring->length > 0);
  Assert(i >= 0 && i < ring->length);

  {
    int n;
    q = ring->head;

    if (i <= ring->length/2) {
      for (n = i; n-- > 0; )
        q = q->rlink;

    } else {
      for (n = ring->length - i; n-- > 0; )
        q = q->llink;
    }
  }

  if (i == 0)
    ring->head = ring->head->rlink;

  x = q->value;

  q->llink->rlink = q->rlink;
  q->rlink->llink = q->llink;

  FREE(q);

  if (--ring->length == 0)
    ring->head = NULL;

  return x;
}

void* Ring_remhi(Ring_T ring)
{
  void *x;
  struct node *q;

  Assert(ring);
  Assert(ring->length > 0);

  q = ring->head->llink;
  x = q->value;

  q->llink->rlink = q->rlink;
  q->rlink->llink = q->llink;

  FREE(q);

  if (--ring->length == 0)
    ring->head = NULL;

  return x;
}

void* Ring_remlo(Ring_T ring)
{
  Assert(ring);
  Assert(ring->length > 0);

  ring->head = ring->head->rlink;

  return Ring_remhi(ring);
}

void Ring_rotate(Ring_T ring, int n)
{
  struct node *q;
  int i;

  Assert(ring);
  Assert(n >= -ring->length && n <= ring->length);

  if (n >= 0)
    i = n % ring->length;
  else
    i = n + ring->length;

  {
    int m;
    q = ring->head;
    if (i <= ring->length/2) {
      for (m = i; m-- > 0; )
        q = q->rlink;

    } else {
      for (m = ring->length - i; m-- > 0; )
        q = q->llink;
    }
  }

  ring->head = q;
}
