/**
 * @file     memory-dev.c
 * @brief    Production version.
 *
 * Nothing special just good practices and an exception
 * `Memory_Failed` is thrown if problem occurs.
 */
#if defined(NDEBUG)

#include "lang/memory.h"

#include <stdlib.h>       /* malloc, calloc, realloc, free */
#include "lang/assert.h"
#include "lang/macros.h"

/* Initialize GLOBAL. Could be thrown from anywhere. */
const Except_T Memory_Failed = { "Memory allocation failed" };

void*
Memory_alloc(size_t nbytes, const char* file, int line)
{
  Require(nbytes > 0);

  void* ptr = malloc(nbytes);

  if (ptr == NULL) {
    if (file == NULL)
    { THROW(Memory_Failed); }
    else
    { Except_throw(&Memory_Failed, file, line); }
  }

  return ptr;
}

void*
Memory_calloc(size_t count, size_t nbytes, const char* file, int line)
{
  Require(count > 0);
  Require(nbytes > 0);

  void* ptr = calloc(count, nbytes);

  if (ptr == NULL) {
    if (file == NULL)
    { THROW(Memory_Failed); }
    else
    { Except_throw(&Memory_Failed, file, line); }
  }
  return ptr;
}

void*
Memory_resize(void* ptr, size_t nbytes, const char* file, int line)
{
  Require(ptr);
  Require(nbytes > 0);

  void* tmp;
  tmp = realloc(ptr, nbytes);

  if (tmp == NULL) {
    free(ptr);
    ptr = NULL;

    if (file == NULL)
    { THROW(Memory_Failed); }
    else
    { Except_throw(&Memory_Failed, file, line); }
  }

  ptr = tmp;
  return ptr;
}

void
Memory_free(void* ptr, const char* file, int line)
{
  /* Used only in the dev version */
  UNUSED(file);
  UNUSED(line);

  if (ptr) {
    free(ptr);
  }
}

#else
typedef int ISO_C_forbids_an_empty_translation_unit;
#endif  /* NDEBUG */
