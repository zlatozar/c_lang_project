/**
 * @file    circ_list.h
 * @brief   Circular List ADT interface.
 */
#if !defined(DATA_STRUCTS_CIRCULAR_LIST_H)
#define DATA_STRUCTS_CIRCULAR_LIST_H

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
 * Insert a new node containing data as first node.
 */
extern void CircList_insert(CircList_T* p_circlist, Generic_T data);

/**
 * Append a new node containing data, as the last item in `p_circlist`
 * Update `p_circlist` to point to the new last node.
 */
extern void CircList_append(CircList_T* p_circlist, Generic_T data);

/**
 * Delete the first node in `p_circlist` and return the data in `p_data__`.
 */
extern bool CircList_delete(CircList_T* p_circlist, Generic_T* p_data__);

#endif  /* DATA_STRUCTS_CIRCULAR_LIST_H */
