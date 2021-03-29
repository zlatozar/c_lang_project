/**
 * @file     memory-dev.c
 * @brief    Memory checked version. Use only during development.
 *
 * Memory allocated through NEW etc. should be freed using FREE macro
 * otherwise strange error message will be raised.
 */
#if !defined(NDEBUG)

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

#define hash(p, ht) (((unsigned long)(p) >> 3) & (sizeof (ht) / sizeof ((ht)[0]) - 1))

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

/* Initialization - points to itself because we build cyclic list. */
static struct descriptor freelist = { .free = &freelist };

typedef struct descriptor* Descriptor_T;

static Descriptor_T
__find(const void* ptr)
{
  Descriptor_T desc = htab[hash(ptr, htab)];

  while (desc && desc->ptr != ptr) {
    desc = desc->link;  /* `link` pointer could be NULL. */
  }

  return desc;
}

static Descriptor_T
__dalloc(void* ptr, size_t size, const char* file, int line)
{
  Require(ptr);

  /* Global variables that hold the state. */
  static Descriptor_T avail;
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

/* __________________________________________________________________________ */

void*
Memory_alloc(size_t nbytes, const char* file, int line)
{
  Require(nbytes > 0);

  /* Note that the number of bytes are multiple of the align size */
  nbytes = ((nbytes + sizeof (union align) - 1) / (sizeof (union align))) *
           (sizeof (union align));

  Descriptor_T desc;
  void* ptr;

  for (desc = freelist.free; desc; desc = desc->free) {

    if (desc->size > nbytes) {
      desc->size -= nbytes;
      /* Cast to make possible pointer arithmetic. */
      ptr = (char*)desc->ptr + desc->size;

      if ((desc = __dalloc(ptr, nbytes, file, line)) != NULL) {
        unsigned h = hash(ptr, htab);
        desc->link = htab[h];
        htab[h] = desc;

        return ptr;

      } else {

        if (file == NULL)
        { THROW(Memory_Failed); }
        else
        { Except_throw(&Memory_Failed, file, line); }
      }
    }

    /* If no suitable chunk was found in the `freelist` circle. */
    if (desc == &freelist) {
      Descriptor_T new_desc = NULL;

      if ((ptr = malloc(nbytes + NALLOC)) == NULL
          || (new_desc = __dalloc(ptr, nbytes + NALLOC, __FILE__, __LINE__)) == NULL) {

        if (ptr != NULL) {
          free(ptr);
          ptr = NULL;
        }

        if (file == NULL)
        { THROW(Memory_Failed); }
        else
        { Except_throw(&Memory_Failed, file, line); }
      }

      Ensure(new_desc);

      new_desc->free = freelist.free;
      freelist.free = new_desc;
    }
  }

  /* Assume impossible. */
  Assert(0);
  return NULL;
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

void*
Memory_resize(void* ptr, size_t nbytes, const char* file, int line)
{
  Require(ptr);
  Require(nbytes > 0);

  Descriptor_T desc = NULL;

  if (((unsigned long)ptr) % (sizeof (union align)) != 0
      || (desc = __find(ptr)) == NULL || desc->free) {

    Except_throw(&Assert_Failed, file, line);
  }

  Ensure(desc);

  void* newptr = Memory_alloc(nbytes, file, line);
  memcpy(newptr, ptr, nbytes < desc->size ? nbytes : desc->size);

  Memory_free(ptr, file, line);

  return newptr;
}

void
Memory_free(void* ptr, const char* file, int line)
{
  if (ptr) {
    Descriptor_T desc = NULL;

    if (((unsigned long)ptr) % (sizeof (union align)) != 0
        || (desc = __find(ptr)) == NULL || desc->free) {

      Except_throw(&Assert_Failed, file, line);
    }

    Ensure(desc);

    desc->free = freelist.free;
    freelist.free = desc;
  }
}

#else
typedef int ISO_C_forbids_an_empty_translation_unit;
#endif  /* NDEBUG */
