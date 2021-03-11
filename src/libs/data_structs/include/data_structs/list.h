/**
 * @file    list.h
 * @brief   Linked List ADT interface.
 */
#if !defined(DATA_STRUCTS_LIST_H)
#define DATA_STRUCTS_LIST_H

#include <stdbool.h>    /* bool   */
#include <stddef.h>     /* size_t */
#include "lang/extend.h"


struct node {
  Generic_T datapointer;
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
extern mem_status List_allocate_node(List_T* p_List, Generic_T p_data);
extern void List_free_node(node_t** pp_node);

/**
 * Initialize a list by setting the list pointer to NULL.
 */
extern List_T List_new(void);

/**
 * Return `true` if given list is an empty list, `false` otherwise.
 */
extern bool List_is_empty(List_T List);

/**
 * Insert a new node containing data as the first item in *p_List.
 */
extern mem_status List_insert(List_T* p_List, Generic_T p_data);

/**
 * Append a new node containing data as the last item in *p_List.
 */
extern mem_status List_append(List_T* p_List, Generic_T p_data);

/**
 * Delete `node` from *p_List. It is checked runtime error if `p_List` is empty.
 */
extern status List_delete_node(List_T* p_List, node_t* p_node);

/**
 * Delete the first node in `*p_List` and return pointer to deleted DATA in
 * `pp_data__` if operation succeed otherwise FAIL. It is checked runtime error
 * if given list is empty. Client should take care to free memory of the
 * returned data.
 */
extern status List_delete_head(List_T* p_List, Generic_T* pp_data__);

/**
 * Call `apply_fn` with the DATA field of each node in `List_T`.
 * If `apply_fn` ever returns FAIL, this function returns FAIL.
 */
extern status List_traverse(List_T list, status (*apply_fn)(Generic_T));

/**
 * @brief    Return each item of list in turn.
 *
 * Return NULL after the last item has been returned. `lastreturn` is the
 * value that was returned last. Note that if `lastreturn` is NULL, start
 * at the beginning of the given list.
 */
extern List_T List_iterator(List_T list, List_T last_return);

/**
 * @brief    Number of list elements.
 */
extern size_t List_length(List_T list);

/**
 * @brief    Find the node in the list that has a data—matching key.
 *
 * If the node is found, it is passed back in `*pp_keynode__`. SUCC is returned.
 * `comp_data_fn` is a comparison function that returns `true` when there is a match.
 */
extern status List_find_key(List_T list, compare_data_FN comp_data_fn, Generic_T key,
                            node_t** pp_keynode__);

/**
 * @brief    Print list data.
 */
extern void List_print(const List_T list, print_data_FN print_data_fn);

/**
 * @brief    Delete every node in the given list.
 *
 * If list is non null, call it with the data stored in each node.
 */
extern void List_destroy(List_T* p_List, free_data_FN free_data_fn);

#endif  /* DATA_STRUCTS_LIST_H */
