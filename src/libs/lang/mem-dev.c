#if !defined(NDEBUG)     /*  Development version */

#include "lang/mem.h"

#include <stdlib.h>      /* malloc, calloc, realloc, free */
#include <string.h>      /* memset                        */
#include "lang/macros.h"
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
const Except_T Mem_Failed = { "Allocation failed" };

//______________________________________________________________________________
//                                                                        Local

LOCAL struct descriptor {
  struct descriptor* free;
  struct descriptor* link;
  const void* ptr;
  long size;
  const char* file;
  int line;
}* htab[TOTAL_BUCKETS];

/* Point to itself because we build cyclic list. */
LOCAL struct descriptor freelist = { .free = &freelist };

LOCAL struct descriptor*
find(const void* ptr)
{
  struct descriptor* bp = htab[hash(ptr, htab)];

  while (bp && bp->ptr != ptr) {
    bp = bp->link;
  }

  return bp;
}

//______________________________________________________________________________

void
Mem_free(void* ptr, const char* file, int line)
{
  if (ptr) {
    struct descriptor* bp = NULL;

    if (((unsigned long)ptr) % (sizeof (union align)) != 0
        || (bp = find(ptr)) == NULL || bp->free) {

      Except_raise(&Assert_Failed, file, line);
    }

    Ensure(bp);

    bp->free = freelist.free;
    freelist.free = bp;
  }
}

void*
Mem_resize(void* ptr, long nbytes, const char* file, int line)
{
  Require(ptr);
  Require(nbytes > 0);

  struct descriptor* bp = NULL;

  if (((unsigned long)ptr) % (sizeof (union align)) != 0
      || (bp = find(ptr)) == NULL || bp->free) {

    Except_raise(&Assert_Failed, file, line);
  }

  Ensure(bp);

  void* newptr = Mem_alloc(nbytes, file, line);
  memcpy(newptr, ptr, nbytes < bp->size ? nbytes : bp->size);

  Mem_free(ptr, file, line);

  return newptr;
}

void*
Mem_calloc(long count, long nbytes, const char* file, int line)
{
  Require(count > 0);
  Require(nbytes > 0);

  void* ptr = Mem_alloc(count * nbytes, file, line);
  memset(ptr, '\0', count * nbytes);

  return ptr;
}

LOCAL struct descriptor*
dalloc(void* ptr, long size, const char* file, int line)
{
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
Mem_alloc(long nbytes, const char* file, int line)
{
  Require(nbytes > 0);

  nbytes = ((nbytes + sizeof (union align) - 1) / (sizeof (union align))) *
           (sizeof (union align));

  struct descriptor* bp;
  void* ptr;

  for (bp = freelist.free; bp; bp = bp->free) {

    if (bp->size > nbytes) {
      bp->size -= nbytes;
      ptr = (char*)bp->ptr + bp->size;

      if ((bp = dalloc(ptr, nbytes, file, line)) != NULL) {
        unsigned h = hash(ptr, htab);
        bp->link = htab[h];
        htab[h] = bp;

        return ptr;

      } else {

        if (file == NULL)
        { THROW(Mem_Failed); }
        else
        { Except_raise(&Mem_Failed, file, line); }
      }
    }

    if (bp == &freelist) {
      struct descriptor* newptr = NULL;

      if ((ptr = malloc(nbytes + NALLOC)) == NULL
          || (newptr = dalloc(ptr, nbytes + NALLOC, __FILE__, __LINE__)) == NULL) {

        if (file == NULL)
        { THROW(Mem_Failed); }
        else
        { Except_raise(&Mem_Failed, file, line); }
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