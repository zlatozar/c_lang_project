/**
 * @file    double_list.h
 * @brief   Double linked List ADT interface.
 */
#if !defined(DATA_STRUCTS_DOUBLE_LIST_H)
#define DATA_STRUCTS_DOUBLE_LIST_H

#include <stddef.h>      /* size_t */
#include "lang/extend.h"

/* Particular node from the double linked list. */
typedef struct double_node doublenode_t;

typedef doublenode_t* DoubleList_T;

/**
 * @brief    Create a new double linked list.
 */
extern DoubleList_T DoubleList_new(void);

/**
 * Return `true` if given list is an empty list, `false` otherwise.
 */
extern bool DoubleList_is_empty(DoubleList_T list);

/**
 * Insert a new node containing data as the first item in `p_list`.
 *
 * Note that `p_list` could point at any element of the list, not
 * just beginning of the list.
 */
extern void DoubleList_insert(DoubleList_T* p_list, Object_T data);

/**
 * Append a new node containing data as the last item in list.
 */
extern void DoubleList_append(DoubleList_T* p_list, Object_T data);

/**
 * @brief    Number of double list elements.
 */
extern size_t DoubleList_length(DoubleList_T list);

/**
 * Return the number `idx` node of `circlist`. The first node is 1. Node that if
 * you want to get the last node, number should be -1. If there aren't enough
 * nodes in the list or it is empty, return NULL.
 */
extern doublenode_t* DoubleList_nth(DoubleList_T list, int idx);

/**
 * Delete the first node in `*p_list` and return pointer to deleted DATA in
 * `p_data__` if operation succeed otherwise `false`. It is checked runtime error
 * if given list is empty. Client should take care to free memory of the
 * returned data.
 */
extern bool DoubleList_delete_head(DoubleList_T* p_list, Object_T* p_data__);

/**
 * Call `apply_fn` with the DATA field of each node in `List_T`.
 * If `apply_fn` ever returns `false`, this function also returns `false`.
 */
extern bool DoubleList_traverse(DoubleList_T list, bool (*apply_fn)(Object_T));

/**
 * Extract the range of nodes `p_start` -- `p_end` including from given double
 * list. `p_start` points to the first element of the double linked list so it
 * presents a double linked list.
 */
extern void DoubleList_cut(DoubleList_T* p_list, doublenode_t** p_start, doublenode_t** p_end);

/**
 * Take `p_source` and put it after `p_target` pointer. Assumes `p_source` is
 * the first node in the list that should be paste it.
 */
extern void DoubleList_paste(DoubleList_T* p_target, DoubleList_T* p_source);

/**
 * @brief    Delete every node in the given list.
 *
 * Use it if data stored in list needs special way of freeing
 * otherwise use `DoubleList_free`.
 */
extern void DoubleList_destroy(DoubleList_T* p_list, free_data_FN free_data_fn);

/**
 * @brief    Free occupied memory.
 */
extern void DoubleList_free(DoubleList_T* p_list);

#endif  /* DATA_STRUCTS_DOUBLE_LIST_H */
