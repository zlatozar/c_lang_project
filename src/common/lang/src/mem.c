#include <stdlib.h>
#include <stddef.h>

#include "common/lang/unused.h"
#include "common/lang/assert.h"
#include "common/lang/except.h"
#include "common/lang/mem.h"

const Except_T Mem_Failed = { "Allocation failed" };

void *Mem_alloc(long nbytes, const char *file, int line) {
  void *ptr;

  Assert(nbytes > 0);

  ptr = malloc(nbytes);

  if (ptr == NULL) {
    if (file == NULL)
      RAISE(Mem_Failed);
    else
      Except_raise(&Mem_Failed, file, line);
  }

  return ptr;
}

void *Mem_calloc(long count, long nbytes,	const char *file, int line) {
  void *ptr;

  Assert(count > 0);
  Assert(nbytes > 0);

  ptr = calloc(count, nbytes);

  if (ptr == NULL) {
    if (file == NULL)
      RAISE(Mem_Failed);
    else
      Except_raise(&Mem_Failed, file, line);
  }

  return ptr;
}

void Mem_free(void *ptr, const char *file, int line) {
  // Used in the dev version
  UNUSED(file);
  UNUSED(line);

  if (ptr)
    free(ptr);
}

void *Mem_resize(void *ptr, long nbytes, const char *file, int line) {
  Assert(ptr);
  Assert(nbytes > 0);

  ptr = realloc(ptr, nbytes);

  if (ptr == NULL) {
    if (file == NULL)
      RAISE(Mem_Failed);
    else
      Except_raise(&Mem_Failed, file, line);
  }

  return ptr;
}
