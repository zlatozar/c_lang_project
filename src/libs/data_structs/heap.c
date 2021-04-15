#include "data_structs/heap.h"

#include "lang/memory.h"
#include "logger/log.h"

const size_t k_resize_step = 128;

struct heap {
  Generic_T* storage;
  size_t nextelement_idx;  /* In this index will be placed next element. */
  size_t size;             /* Current size of the `storage`. */
};

/* ______________________________________________________________________________ */
/*                                                                         Local  */

/*
 * `heap` is a heap except for element. Find the correct place for element by
 * swapping it with its parent if necessary. If a swap is made, `heap` is a heap
 * except for the parents position, so call `__siftup` recursively.
 */
static void
__siftup(Heap_T heap, size_t elm_idx, cmp_data_FN cmp_fn)
{
  /* We reach root. */
  if (elm_idx == 0) {
    return;
  }

  /* Calculate parent position. */
  size_t parent_idx = (elm_idx - 1) / 2;
  int cmp_result = cmp_fn(heap->storage[elm_idx], heap->storage[parent_idx]);

  if (cmp_result >= 0) {
    return;
  }

  Generic_T tmpvalue;

  /* Switch with parent. */
  tmpvalue = heap->storage[elm_idx];
  heap->storage[elm_idx] = heap->storage[parent_idx];
  heap->storage[parent_idx] = tmpvalue;

  /* Do the same with the parent. */
  __siftup(heap, parent_idx, cmp_fn);

  return;
}

/*
 * `heap` is a heap except for parent. Find the correct place for parent by
 * swapping it with the smaller of its children. If a swap is made, `heap` is a
 * heap except for the child's position, so call `__siftdown` recursively.
 */
static void
__siftdown(Heap_T heap, size_t parent_idx, cmp_data_FN cmp_fn)
{
  size_t leftchild = 2 * parent_idx + 1;
  size_t rightchild = leftchild + 1;

  /* No children. */
  if (leftchild >= heap->nextelement_idx)
    return;

  Generic_T tmpvalue;

  /* No right child. */
  int left_cmp = cmp_fn(heap->storage[parent_idx], heap->storage[leftchild]);

  if (rightchild >= heap->nextelement_idx) {

    if (left_cmp > 0) {
       tmpvalue = heap->storage[parent_idx];
       heap->storage[parent_idx] = heap->storage[leftchild];
       heap->storage[leftchild] = tmpvalue;
     }
     return;
  }

  /* Two children. Swap with the smaller child. */
  int right_cmp = cmp_fn(heap->storage[parent_idx], heap->storage[rightchild]);

  if (left_cmp > 0 || right_cmp > 0) {
    int left_right_cmp = cmp_fn(heap->storage[leftchild], heap->storage[rightchild]);

    size_t swapelement = (left_right_cmp < 0) ? leftchild : rightchild;

    tmpvalue = heap->storage[parent_idx];
    heap->storage[parent_idx] = heap->storage[swapelement];
    heap->storage[swapelement] = tmpvalue;

    __siftdown(heap, swapelement, cmp_fn);
  }

  return;
}

/* ______________________________________________________________________________ */

Heap_T
Heap_new()
{
  Heap_T heap;
  NEW(heap);

  heap->storage = ALLOC(k_resize_step * sizeof(Generic_T));

  heap->nextelement_idx = 0;
  heap->size = k_resize_step;

  return heap;
}

bool
Heap_is_empty(Heap_T heap)
{
  return heap->nextelement_idx == 0;
}

/*
 * The data is inserted in the heap by placing it at the end and using
 * `__siftupf` to find its proper position.
 */
void
Heap_insert(Heap_T heap, Generic_T data, cmp_data_FN cmp_fn)
{
  /* Not enough space in the array, so more must be allocated. */
  if(heap->nextelement_idx == heap->size) {

    Generic_T* newstorage =
        RESIZE(heap->storage,
               (heap->size + k_resize_step) * sizeof(*newstorage));

    heap->storage = newstorage;
    heap->size += k_resize_step;
  }

  /* Add at the end then move to the proper place. */
  heap->storage[heap->nextelement_idx] = data;
  __siftup(heap, heap->nextelement_idx, cmp_fn);

  heap->nextelement_idx++;
}

/*
 * The data is deleted by placing the last element in its place and using
 * `__siftdown` to find its proper position.
 */
bool
Heap_delete(Heap_T heap, size_t elm_idx, cmp_data_FN cmp_fn, Generic_T* p_data__)
{
  if (elm_idx >= heap->nextelement_idx) {
    Log_error("Given elm_idx=%zd is out of heap scope.", elm_idx);
    return false;
  }

  *p_data__ = heap->storage[elm_idx];

  heap->nextelement_idx--;

  /* The easiest case is if delete the last element in the heap storage. */
  if (elm_idx != heap->nextelement_idx) {
    heap->storage[elm_idx] = heap->storage[heap->nextelement_idx];
    __siftdown(heap, elm_idx, cmp_fn);
  }

  return true;
}
