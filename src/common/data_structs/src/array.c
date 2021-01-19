#include <string.h>  /* memcpy */

#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/array.h"
#include "common/data_structs/arrayrep.h"

void ArrayRep_init(struct array_rep* p_arrayRep, size_t length, size_t size, void* mem_alloc)
{
  Assert(p_arrayRep);
  Assert((length > 0 && mem_alloc) || (length == 0 && mem_alloc == NULL));
  Assert(size > 0);

  p_arrayRep->length = length;
  p_arrayRep->size = size;

  if (length > 0)
    p_arrayRep->mem_alloc = mem_alloc;
  else
    p_arrayRep->mem_alloc = NULL;
}

Array_T Array_new(size_t length, size_t size)
{
  Array_T arrayT;
  NEW(arrayT);

  if (length > 0)
    ArrayRep_init(arrayT, length, size, CALLOC(length, size));
  else
    ArrayRep_init(arrayT, length, size, NULL);

  return arrayT;
}

void Array_free(Array_T* self)
{
  Assert(self && *self);

  FREE((*self)->mem_alloc);
  FREE(*self);
}

size_t Array_length(Array_T self)
{
  Assert(self);
  return self->length;
}

size_t Array_size(Array_T self)
{
  Assert(self);
  return self->size;
}

void* Array_get(Array_T self, unsigned i)
{
  Assert(self);
  Assert(i < self->length);

  return self->mem_alloc + i * self->size;
}

void* Array_put(Array_T self, unsigned i, void* elem)
{
  Assert(self);
  Assert(i < self->length);
  Assert(elem);

  memcpy(self->mem_alloc + i*self->size, elem, self->size);

  return elem;
}

void Array_resize(Array_T self, size_t length)
{
  Assert(self);

  if (length == 0)
    FREE(self->mem_alloc);

  else if (self->length == 0)
    self->mem_alloc = ALLOC(length * self->size);
  else
    RESIZE(self->mem_alloc, length * self->size);

  self->length = length;
}

Array_T Array_copy(Array_T self, size_t length)
{
  Array_T arrayT;

  Assert(self);

  arrayT = Array_new(length, self->size);

  if (arrayT->length >= self->length && self->length > 0)
    memcpy(arrayT->mem_alloc, self->mem_alloc, self->length * self->size);

  else if (self->length > arrayT->length && arrayT->length > 0)
    memcpy(arrayT->mem_alloc, self->mem_alloc, arrayT->length * self->size);

  return arrayT;
}
