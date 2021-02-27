/**
 * @file    list.h
 * @brief   Linked List ADT interface.
 */
#if !defined(DATA_STRUCTS_LIST_H)
#define DATA_STRUCTS_LIST_H

#include <stdbool.h>     /* bool                    */
#include "lang/extend.h" /* generic_ptr, mem_status */

#ifdef __cplusplus
extern "C" {
#endif        /* __cplusplus */

/* Particular node from the list */
typedef struct node node_t;

/* Interpret `List_T list as a whole, `node_t*` is a pointer to a note. */
typedef node_t* List_T;

/**
 * Allocate node with the size of the notes of a given list.
 *
 * @throws `Mem_Failed` exception if not succeed.
 */
extern mem_status List_allocate_node(List_T* p_List, generic_ptr data);
extern void List_free_node(List_T* p_List);

/**
 * Initialize *p_List by setting the list pointer to NULL.
 */
extern void List_init(List_T* p_List);

/**
 * Return `true` if given list is an empty list, `false` otherwise.
 */
extern bool List_is_empty(List_T List);

/**
 * Insert a new node containing data as the first item in *p_List.
 */
extern mem_status List_insert(List_T* p_List, generic_ptr data);

/**
 * Append a new node containing data as the last item in *p_List.
 */
extern void List_append(List_T* p_List, generic_ptr data);

/**
 * Delete `node` from *p_List. It is checked runtime error
 * if `p_List` is empty.
 */
extern bool List_delete_node(List_T* p_List, node_t* p_node);

/**
 * Delete the first node in `*p_List` and return the DATA in `p_data_`.
 * It is checked runtime error if given list is empty.
 *
 * @param p_data_[out]    deleted node data
 */
extern bool List_delete(List_T* p_List, generic_ptr* p_data_);


#ifdef __cplusplus
}
#endif        /* __cplusplus */

#endif  /* DATA_STRUCTS_LIST_H */
