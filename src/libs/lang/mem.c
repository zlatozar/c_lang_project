#if defined(NDEBUG)       /* Production version */

#include "lang/mem.h"

#include <stdlib.h>       /* malloc, calloc, realloc, free */
#include "lang/macros.h"
#include "lang/assert.h"

/* Initialize GLOBAL. Could be thrown from anywhere. */
const Except_T Mem_Failed = { "Memory allocation failed" };

void*
Mem_alloc(long nbytes, const char* file, int line)
{
  Require(nbytes > 0);

  void* ptr = malloc(nbytes);

  if (ptr == NULL) {
    if (file == NULL)
    { THROW(Mem_Failed); }
    else
    { Except_raise(&Mem_Failed, file, line); }
  }
  return ptr;
}

void*
Mem_calloc(long count, long nbytes, const char* file, int line)
{
  Require(count > 0);
  Require(nbytes > 0);

  void* ptr = calloc(count, nbytes);

  if (ptr == NULL) {
    if (file == NULL)
    { THROW(Mem_Failed); }
    else
    { Except_raise(&Mem_Failed, file, line); }
  }
  return ptr;
}

void
Mem_free(void* ptr, const char* file, int line)
{
  /* Used only in the dev version */
  UNUSED(file);
  UNUSED(line);

  if (ptr)
  { free(ptr); }
}

void*
Mem_resize(void* ptr, long nbytes, const char* file, int line)
{
  Require(ptr);
  Require(nbytes > 0);

  ptr = realloc(ptr, nbytes);

  if (ptr == NULL) {
    if (file == NULL)
    { THROW(Mem_Failed); }
    else
    { Except_raise(&Mem_Failed, file, line); }
  }

  return ptr;
}

#else
typedef int ISO_C_forbids_an_empty_translation_unit;
#endif  /* NDEBUG */
