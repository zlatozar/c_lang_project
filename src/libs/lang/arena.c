#include "lang/arena.h"

#include <stdlib.h>      /* malloc, free */
#include <string.h>      /* memset       */
#include "lang/assert.h"

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

static Arena_T freechunks;

static int nfree;

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
Arena_alloc(Arena_T arena, size_t nbytes, const char* file, int line)
{
  Require(arena);
  Require(nbytes > 0);

  nbytes = ((nbytes + sizeof (union align) - 1) /
            (sizeof (union align))) * (sizeof (union align));

  /* Current initial state. */
  long space_left = arena->limit - arena->avail;

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
        { Except_throw(&Arena_Failed, file, line); }
      }

      limit = (char*)ptr + m;
    }
    *ptr = *arena;

    arena->avail = (char*)((union header*)ptr + 1);
    arena->limit = limit;
    arena->prev  = ptr;

    space_left = arena->limit - arena->avail;
  }

  arena->avail += nbytes;

  return arena->avail - nbytes;
}

void*
Arena_calloc(Arena_T arena, size_t count, size_t nbytes,
             const char* file, int line)
{
  Require(count > 0);

  void* ptr = Arena_alloc(arena, count * nbytes, file, line);
  memset(ptr, '\0', count * nbytes);

  return ptr;
}

void
Arena_dispose(Arena_T* p_arena)
{
  Require(p_arena && *p_arena);

  Arena_free(*p_arena);
  free(*p_arena);
  *p_arena = NULL;
}

void
Arena_free(Arena_T arena)
{
  Require(arena);

  while (arena->prev) {
    struct arena tmp = *arena->prev;

    if (nfree < THRESHOLD) {
      arena->prev->prev = freechunks;
      freechunks = arena->prev;
      nfree++;
      freechunks->limit = arena->limit;

    } else {
      free(arena->prev);
    }

    *arena = tmp;
  }

  Ensure(arena->limit == NULL);
  Ensure(arena->avail == NULL);
}
