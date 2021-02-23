#include "lang/arena.h"

#include <stdlib.h>
#include <string.h>
#include "lang/assert.h"
#include "lang/macros.h"

/* Initialize exceptions */
const Except_T Arena_NewFailed = { "Arena creation failed" };
const Except_T Arena_Failed = { "Arena allocation failed" };

#define THRESHOLD 10

/* __________________________________________________________________________ */
/*                                                                     Local  */

struct arena {
  struct arena* prev;
  char* avail;
  char* limit;
};

union align {
  int i;
  long l;
  long* lp;
  void* p;
  void (*fp)(void);
  float f;
  double d;
  long double ld;
};

union header {
  struct arean* b;
  union align a;
};

LOCAL Arena_T freechunks;

LOCAL int nfree;

/* __________________________________________________________________________ */

Arena_T
Arena_new(void)
{
  Arena_T arena = malloc(sizeof (*arena));

  if (arena == NULL)
  { THROW(Arena_NewFailed); }

  arena->prev = NULL;
  arena->limit = arena->avail = NULL;

  return arena;
}

void*
Arena_alloc(Arena_T self, size_t nbytes, const char* file, int line)
{
  Require(self);
  Require(nbytes > 0);

  nbytes = ((nbytes + sizeof (union align) - 1) /
            (sizeof (union align))) * (sizeof (union align));

  /* Current initial state. */
  long space_left = self->limit - self->avail;

  while (space_left > 0 && nbytes > (unsigned long)space_left) {
    Arena_T ptr;
    char* limit;

    if ((ptr = freechunks) != NULL) {
      freechunks = freechunks->prev;
      nfree--;
      limit = ptr->limit;

    } else {
      size_t m = sizeof (union header) + nbytes + 10 * 1024;
      ptr = malloc(m);

      if (ptr == NULL) {
        if (file == NULL)
        { THROW(Arena_Failed); }
        else
        { Except_raise(&Arena_Failed, file, line); }
      }

      limit = (char*)ptr + m;
    }
    *ptr = *self;

    self->avail = (char*)((union header*)ptr + 1);
    self->limit = limit;
    self->prev  = ptr;

    space_left = self->limit - self->avail;
  }

  self->avail += nbytes;

  return self->avail - nbytes;
}

void*
Arena_calloc(Arena_T self, size_t count, size_t nbytes,
             const char* file, int line)
{
  Require(count > 0);

  void* ptr = Arena_alloc(self, count * nbytes, file, line);
  memset(ptr, '\0', count * nbytes);

  return ptr;
}

void
Arena_dispose(Arena_T* p_self)
{
  Require(p_self && *p_self);

  Arena_free(*p_self);
  free(*p_self);
  *p_self = NULL;
}

void
Arena_free(Arena_T self)
{
  Require(self);

  while (self->prev) {
    struct arena tmp = *self->prev;

    if (nfree < THRESHOLD) {
      self->prev->prev = freechunks;
      freechunks = self->prev;
      nfree++;
      freechunks->limit = self->limit;

    } else {
      free(self->prev);
    }

    *self = tmp;
  }

  Ensure(self->limit == NULL);
  Ensure(self->avail == NULL);
}
