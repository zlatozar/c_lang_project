/**
 * @file    heap.h
 * @brief   Heap ADT interface.
 */
#if !defined(DATA_STRUCTS_HEAP_H)
#define DATA_STRUCTS_HEAP_H

#include <stddef.h>
#include "lang/extend.h"

typedef struct heap* Heap_T;

/**
 * Initialize a heap by allocating the array.
 */
extern Heap_T Heap_new(void);

/**
 * Return true if heap is an empty.
 */
extern bool Heap_is_empty(Heap_T heap);

/**
 * @brief    Insert data into heap.
 *
 * `cmp_fn` is a comparison function that returns a value less than 0 if its
 * first argument is less than its second, 0 if the arguments are equal.
 * Otherwise, `cmp_fn` returns a value greater than 0.
 */
extern void Heap_insert(Heap_T heap, Generic_T data, cmp_data_FN cmp_fn);

/**
 * @breif    Delete element from heap.
 *
 * Store the data, from the deleted node in `p_data__`. `cmp_fn` is a comparison
 * function that returns a value less than 0 if its first argument is less than
 * its second, 0 if the arguments are equal. Otherwise, `cmp_fn` returns a value
 * greater than 0.
 */
extern bool Heap_delete(Heap_T heap, size_t elm_idx, cmp_data_FN cmp_fn,
                        Generic_T* p_data__);


#endif  /* DATA_STRUCTS_HEAP_H */
