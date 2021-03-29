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
 * Insert a new node containing data.
 */
extern void CircList_insert(CircList_T* p_circlist, Generic_T p_data);


#endif  /* DATA_STRUCTS_CIRCULAR_LIST_H */
