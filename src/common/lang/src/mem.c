#include <stdlib.h>  /* malloc, calloc, realloc, free */

#include "common/lang/unused.h"
#include "common/lang/assert.h"
#include "common/lang/mem.h"

/* Define expected exception */
const Except_T Mem_Failed = { "Memory allocation failed" };

void* Mem_alloc(size_t nbytes, const char* file, unsigned line) {
  void* ptr;

  Assert(nbytes > 0);

  ptr = malloc(nbytes);

  if (ptr == NULL) {
    if (file == NULL)
      THROW(Mem_Failed);
    else
      Except_raise(&Mem_Failed, file, line);
  }

  return ptr;
}

void* Mem_calloc(size_t count, size_t nbytes,	const char* file, unsigned line) {
  void* ptr;

  Assert(count > 0);
  Assert(nbytes > 0);

  ptr = calloc(count, nbytes);

  if (ptr == NULL) {
    if (file == NULL)
      THROW(Mem_Failed);
    else
      Except_raise(&Mem_Failed, file, line);
  }

  return ptr;
}

void Mem_free(void* ptr, const char* file, unsigned line) {
  // Used only in the dev version
  UNUSED(file);
  UNUSED(line);

  if (ptr)
    free(ptr);
}

void* Mem_resize(void* ptr, size_t nbytes, const char* file, unsigned line) {
  Assert(ptr);
  Assert(nbytes > 0);

  ptr = realloc(ptr, nbytes);

  if (ptr == NULL) {
    if (file == NULL)
      THROW(Mem_Failed);
    else
      Except_raise(&Mem_Failed, file, line);
  }

  return ptr;
}
