/**
 * @file    list.h
 * @brief   Linked List ADT interface.
 */
#if !defined(DATA_STRUCTS_LIST_H)
#define DATA_STRUCTS_LIST_H

#include <stdbool.h>     /* bool */
#include "lang/extend.h"

#ifdef __cplusplus
extern "C" {
#endif        /* __cplusplus */


struct node {
  generic_ptr datapointer;
  struct node* next;
};

/* Particular node from the list. */
typedef struct node node_t;

/*
 * For readability `List_T` and `node_t*` could be used. Use `List_T` when you
 * mean list as a whole and `note_t*` respectively when you deal with a pointer
 * to a note.
 */
typedef node_t* List_T;

#define DATA(p_node) ((p_node)->datapointer)
#define NEXT(p_node) ((p_node)->next)

/**
 * Allocate node with the size of the notes of a given list.
 *
 * @throws `Mem_Failed` exception if not succeed.
 */
extern mem_status List_allocate_node(List_T* p_List, generic_ptr p_data);
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
extern mem_status List_insert(List_T* p_List, generic_ptr p_data);

/**
 * Append a new node containing data as the last item in *p_List.
 */
extern void List_append(List_T* p_List, generic_ptr p_data);

/**
 * Delete `node` from *p_List. It is checked runtime error
 * if `p_List` is empty.
 */
extern bool List_delete_node(List_T* p_List, node_t* p_node);

/**
 * Delete the first node in `*p_List` and return deleted DATA if operation
 * succeed otherwise NULL. It is checked runtime error if given list is empty.
 */
extern generic_ptr List_delete(List_T* p_List);

/**
 * Call `apply_fn` with the DATA field of each node in `List_T`.
 * If `apply_fn` ever returns ERROR, this function returns ERROR.
 */
extern bool List_traverse(List_T self, bool (*apply_fn)(generic_ptr));

/**
 * @brief    Return each item of list in turn.
 *
 * Return NULL after the last item has been returned. `lastreturn` is the
 * value that was returned last. Note that if `lastreturn` is NULL, start
 * at the beginning of the given list.
 */
extern List_T List_iterator(List_T self, List_T last_return);

/**
 * @brief    Find the node in the list that has a data—matching key.
 *
 * If the node is found, it is passed back in `*p_keynode`. True is returned.
 * `comp_fn` is a comparison function that returns `true` when there is a match.
 */
extern bool List_find_key(List_T self, compare_data_FN comp_data_fn, generic_ptr key, node_t** p_keynode);

/**
 * @brief    Delete every node in the given list.
 *
 * If list is non null, call it with the data stored in each node.
 */
extern void List_destroy(List_T* p_List, free_data_FN free_data_fn);

#ifdef __cplusplus
}
#endif        /* __cplusplus */

#endif  /* DATA_STRUCTS_LIST_H */
