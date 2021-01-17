#include <string.h>  /* memcpy */

#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/array.h"
#include "common/data_structs/arrayrep.h"

void ArrayRep_init(Array_T array, size_t length, size_t size, void* mem_alloc)
{
  Assert(array);
  Assert((mem_alloc && length > 0) || (length == 0 && mem_alloc == NULL));
  Assert(size > 0);

  array->length = length;
  array->size   = size;

  if (length > 0)
    array->mem_alloc = mem_alloc;
  else
    array->mem_alloc = NULL;
}

Array_T Array_new(size_t length, size_t size)
{
  Array_T array;
  NEW(array);

  if (length > 0)
    ArrayRep_init(array, length, size, CALLOC(length, size));
  else
    ArrayRep_init(array, length, size, NULL);

  return array;
}

void Array_free(Array_T* array)
{
  Assert(array && *array);

  FREE((*array)->mem_alloc);
  FREE(*array);
}

size_t Array_length(Array_T array)
{
  Assert(array);
  return array->length;
}

size_t Array_size(Array_T array)
{
  Assert(array);
  return array->size;
}

void* Array_get(Array_T array, unsigned i)
{
  Assert(array);
  Assert(i < array->length);

  return array->mem_alloc + i*array->size;
}

void* Array_put(Array_T array, unsigned i, void* elem)
{
  Assert(array);
  Assert(i < array->length);
  Assert(elem);

  memcpy(array->mem_alloc + i*array->size, elem, array->size);

  return elem;
}

void Array_resize(Array_T array, size_t length)
{
  Assert(array);

  if (length == 0)
    FREE(array->mem_alloc);

  else if (array->length == 0)
    array->mem_alloc = ALLOC(length*array->size);
  else
    RESIZE(array->mem_alloc, length*array->size);

  array->length = length;
}

Array_T Array_copy(Array_T array, size_t length)
{
  Array_T copy;

  Assert(array);

  copy = Array_new(length, array->size);

  if (copy->length >= array->length && array->length > 0)
    memcpy(copy->mem_alloc, array->mem_alloc, array->length * array->size);

  else if (array->length > copy->length && copy->length > 0)
    memcpy(copy->mem_alloc, array->mem_alloc, copy->length * array->size);

  return copy;
}
