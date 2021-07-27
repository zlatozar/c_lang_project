/**
 * @file    list.h
 * @brief   List data structure.
 */
#if !defined(DATA_STRUCTS_LIST_REP_H)
#define DATA_STRUCTS_LIST_REP_H

#include "lang/extend.h"

struct node {
  Object_T datapointer;
  struct node* next;
};

/* Particular node from the list. */
typedef struct node node_t;

/*
 * For readability both `List_T` and `node_t*` could be used. Use `List_T` when
 * you mean list as a whole and `note_t*` respectively when you deal with a
 * pointer to a node. `node_t` also represents some part of the list - from it to
 * the end.
 */
typedef node_t* List_T;
typedef node_t* CircList_T;

#define DATA(p_node) ((p_node)->datapointer)
#define NEXT(p_node) ((p_node)->next)

/* __________________________________________________________________________ */
/*                                                          Helper functions  */

extern void List__allocate_node(node_t** pp_node, Object_T data);

extern void List__free_node(node_t** PP_nod);

#endif  /* DATA_STRUCTS_LIST_REP_H */
