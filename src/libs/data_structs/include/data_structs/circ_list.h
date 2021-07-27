/**
 * @file    circ_list.h
 * @brief   Circular List ADT interface.
 */
#if !defined(DATA_STRUCTS_CIRCULAR_LIST_H)
#define DATA_STRUCTS_CIRCULAR_LIST_H

#include <stddef.h>    /* size_t */
#include "list_rep.h"

/**
 * @brief    Create a new circular list.
 */
extern CircList_T CircList_new(void);

/**
 * Return `true` if given circular list is an empty list,
 * `false` otherwise.
 */
extern bool CircList_is_empty(CircList_T circlist);

/**
 * Insert a new node containing data as first node after the given `p_circlist`
 * pointer. Expand in clockwise direction.
 */
extern void CircList_insert(CircList_T* p_circlist, Object_T data);

/**
 * Append a new node containing data, as the last item in `p_circlist`
 * Update `p_circlist` to point to the new last node. Expand counter clockwise.
 */
extern void CircList_append(CircList_T* p_circlist, Object_T data);

/**
 * Delete the first node in `p_circlist` and return the data in `p_data__`.
 */
extern bool CircList_delete(CircList_T* p_circlist, Object_T* p_data__);

/**
 * Call `apply_fn` with the DATA field of each node in `CircList_T`.
 * If `apply_fn` ever returns `false`, this function also returns `false`.
 * NOTE: Traverse circular list in clockwise direction.
 *   If you always `CircList_append` - traverse will do FIFO,
 *   if `CircList_insert` - LIFO.
 */
extern bool CircList_traverse(CircList_T circlist, bool (*apply_fn)(Object_T));

/**
 * Return the number `idx` node of `circlist`. The first node is 1. Node that if
 * you want to get the last node, number should be -1. If there aren't enough
 * nodes in the list or it is empty, return NULL.
 */
extern node_t* CircList_nth(CircList_T circlist, int idx);

/**
 * @brief    Return each item of list in turn.
 *
 * Return NULL after the last item has been returned. `lastreturn` is the
 * value that was returned last. Note that if `lastreturn` is NULL, start
 * at the beginning of the given list.
 */
extern node_t* CircList_iterator(CircList_T circlist, node_t* last_return);

/**
 * @brief    Number of list elements.
 */
extern size_t CircList_length(CircList_T circlist);

/**
 * @brief    Delete every node in the given list.
 *
 * Use it if data stored in list needs special way of freeing
 * otherwise use `List_free`.
 */
extern void CircList_destroy(CircList_T* p_circlist, free_data_FN free_data_fn);

extern void CircList_free(CircList_T* p_circlist);

#endif  /* DATA_STRUCTS_CIRCULAR_LIST_H */
