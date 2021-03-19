#if !defined(NDEBUG)     /* Memory checked version */

#include "lang/memory.h"

#include <stdlib.h>      /* malloc, calloc, realloc, free */
#include <string.h>      /* memset                        */
#include "lang/assert.h"

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

#define TOTAL_BUCKETS 2048
#define NDESCRIPTORS 512

#define hash(p, t) (((unsigned long)(p) >> 3) & (sizeof (t) / sizeof ((t)[0]) - 1))

#define NALLOC (((TOTAL_BUCKETS * 2) + sizeof (union align) - 1) / (sizeof (union align))) \
  * (sizeof (union align))

/* Initialize GLOBAL. Could be thrown from anywhere. */
const Except_T Memory_Failed = { "Allocation failed" };

/* __________________________________________________________________________ */
/*                                                                     Local  */

static struct descriptor {
  struct descriptor* free;
  struct descriptor* link;
  const void* ptr;
  size_t size;
  const char* file;
  int line;
}* htab[TOTAL_BUCKETS];

/* Point to itself because we build cyclic list. */
static struct descriptor freelist = { .free = &freelist };

static struct descriptor*
__find(const void* ptr)
{
  struct descriptor* bp = htab[hash(ptr, htab)];

  while (bp && bp->ptr != ptr) {
    bp = bp->link;
  }

  return bp;
}

/* __________________________________________________________________________ */

void
Memory_free(void* ptr, const char* file, int line)
{
  if (ptr) {
    struct descriptor* bp = NULL;

    if (((unsigned long)ptr) % (sizeof (union align)) != 0
        || (bp = __find(ptr)) == NULL || bp->free) {

      Except_throw(&Assert_Failed, file, line);
    }

    Ensure(bp);

    bp->free = freelist.free;
    freelist.free = bp;
  }
}

void*
Memory_resize(void* ptr, size_t nbytes, const char* file, int line)
{
  Require(ptr);
  Require(nbytes > 0);

  struct descriptor* bp = NULL;

  if (((unsigned long)ptr) % (sizeof (union align)) != 0
      || (bp = __find(ptr)) == NULL || bp->free) {

    Except_throw(&Assert_Failed, file, line);
  }

  Ensure(bp);

  void* newptr = Memory_alloc(nbytes, file, line);
  memcpy(newptr, ptr, nbytes < bp->size ? nbytes : bp->size);

  Memory_free(ptr, file, line);

  return newptr;
}

void*
Memory_calloc(size_t count, size_t nbytes, const char* file, int line)
{
  Require(count > 0);
  Require(nbytes > 0);

  void* ptr = Memory_alloc(count * nbytes, file, line);
  memset(ptr, '\0', count * nbytes);

  return ptr;
}

static struct descriptor*
__dalloc(void* ptr, size_t size, const char* file, int line)
{
  Require(ptr);

  static struct descriptor* avail;
  static int nleft;

  if (nleft <= 0) {
    avail = malloc(NDESCRIPTORS * sizeof (*avail));

    if (avail == NULL)
    { return NULL; }

    nleft = NDESCRIPTORS;
  }

  avail->ptr  = ptr;
  avail->size = size;
  avail->file = file;
  avail->line = line;
  avail->free = avail->link = NULL;

  nleft--;

  return avail++;
}

void*
Memory_alloc(size_t nbytes, const char* file, int line)
{
  Require(nbytes > 0);

  /* Note that the number of bytes are multiple of the align size */
  nbytes = ((nbytes + sizeof (union align) - 1) / (sizeof (union align))) *
           (sizeof (union align));

  struct descriptor* bp;
  void* ptr;

  for (bp = freelist.free; bp; bp = bp->free) {

    if (bp->size > nbytes) {
      bp->size -= nbytes;
      ptr = (char*)bp->ptr + bp->size;

      if ((bp = __dalloc(ptr, nbytes, file, line)) != NULL) {
        unsigned h = hash(ptr, htab);
        bp->link = htab[h];
        htab[h] = bp;

        return ptr;

      } else {

        if (file == NULL)
        { THROW(Memory_Failed); }
        else
        { Except_throw(&Memory_Failed, file, line); }
      }
    }

    /* No suitable chuck was found in the `freelist` circle */
    if (bp == &freelist) {
      struct descriptor* newptr = NULL;

      if ((ptr = malloc(nbytes + NALLOC)) == NULL
          || (newptr = __dalloc(ptr, nbytes + NALLOC, __FILE__, __LINE__)) == NULL) {

        if (ptr != NULL) {
          free(ptr);
          ptr = NULL;
        }

        if (file == NULL)
        { THROW(Memory_Failed); }
        else
        { Except_throw(&Memory_Failed, file, line); }
      }

      Ensure(newptr);

      newptr->free = freelist.free;
      freelist.free = newptr;
    }
  }

  Assert(0);
  return NULL;
}

#else
typedef int ISO_C_forbids_an_empty_translation_unit;
#endif  /* NDEBUG */
