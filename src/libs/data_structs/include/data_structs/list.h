/**
 * @file    list.h
 * @brief   Linked List ADT interface.
 */
#if !defined(DATA_STRUCTS_LIST_H)
#define DATA_STRUCTS_LIST_H

#include <stdbool.h>    /* bool   */
#include <stddef.h>     /* size_t */
#include "list_rep.h"

/**
 * @brief    Create a new list.
 */
extern List_T List_new(void);

/**
 * Return `true` if given list is an empty list, `false` otherwise.
 */
extern bool List_is_empty(List_T List);

/**
 * Insert a new node containing data as the first item in *p_List.
 */
extern void List_insert(List_T* p_List, Generic_T p_data);

/**
 * Append a new node containing data as the last item in *p_List.
 */
extern void List_append(List_T* p_List, Generic_T p_data);

/**
 * Delete given `p_node` from *p_List. It is checked runtime error
 * if `p_List` is empty.
 */
extern bool List_delete_node(List_T* p_List, node_t* p_node);

/**
 * Delete the first node in `*p_List` and return pointer to deleted DATA in
 * `pp_data__` if operation succeed otherwise `false`. It is checked runtime error
 * if given list is empty. Client should take care to free memory of the
 * returned data.
 */
extern bool List_delete_head(List_T* p_List, Generic_T* pp_data__);

/**
 * Call `apply_fn` with the DATA field of each node in `List_T`.
 * If `apply_fn` ever returns `false`, this function also returns `false`.
 */
extern bool List_traverse(List_T list, bool (*apply_fn)(Generic_T));

/**
 * @brief    Return each item of list in turn.
 *
 * Return NULL after the last item has been returned. `lastreturn` is the
 * value that was returned last. Note that if `lastreturn` is NULL, start
 * at the beginning of the given list.
 */
extern List_T List_iterator(List_T list, node_t* last_return);

/**
 * @brief    Number of list elements.
 */
extern size_t List_length(List_T list);

/**
 * @brief    Find the node in the list that has a data—matching key.
 *
 * If the node is found, it is passed back in `*pp_keynode__` - `true` is returned.
 * `comp_data_fn` is a comparison function that returns `true` when there is a match.
 */
extern bool List_find_key(List_T list, compare_data_FN comp_data_fn, Generic_T key,
                          node_t** pp_keynode__);

/**
 * @brief    Print list data.
 */
extern void List_print(const List_T list, print_data_FN print_data_fn);

/**
 * @brief    Delete every node in the given list.
 *
 * Use it if data stored in list needs special way of freeing
 * otherwise use `List_free`.
 */
extern void List_destroy(List_T* p_List, free_data_FN free_data_fn);

/**
 * @brief    Free occupied memory.
 */
extern void List_free(List_T* p_List);

#endif  /* DATA_STRUCTS_LIST_H */
