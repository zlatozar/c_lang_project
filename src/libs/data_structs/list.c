#include "data_structs/list.h"

#include <stdlib.h>       /* malloc, free */
#include "lang/assert.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                    Public  */

mem_status
List_allocate_node(List_T* p_List, generic_ptr p_data)
{
  node_t* p_node = malloc(sizeof(node_t));
  if (p_node == NULL) {
    Log_error("Root cause - 'malloc' do not succeed.");
    return ERROR;
  }

  *p_List = p_node;

  DATA(p_node) = p_data;
  NEXT(p_node) = NULL;

  return OK;
}

void
List_free_node(node_t** p_node)
{
  free(*p_node);
  *p_node = NULL;
}

void
List_init(List_T* p_List)
{
  *p_List = NULL;
}

bool
List_is_empty(List_T self)
{
  return (self == NULL) ? true : false;
}

mem_status
List_insert(List_T* p_List, generic_ptr p_data)
{
  node_t* p_node;
  if (List_allocate_node(&p_node, p_data) == ERROR) {
    Log_error("     Can't insert node.");
    return ERROR;
  }

  NEXT(p_node) = *p_List;
  *p_List = p_node;

  return OK;
}

mem_status
List_append(List_T* p_List, generic_ptr p_data)
{
  node_t* p_node;

  if (List_allocate_node(&p_node, p_data) == ERROR) {
    Log_error("     Can't append to list.");
    return ERROR;
  }

  if (List_is_empty(*p_List) == true) {
    *p_List = p_node;

  } else {
    node_t* iter_node;
    for (iter_node = *p_List; NEXT(iter_node) != NULL; ) {
      iter_node = NEXT(iter_node);
    }

    NEXT(iter_node) = p_node;
  }

  return OK;
}

status
List_delete_node(List_T* p_List, node_t* p_node)
{
  Require(*p_List);

  if (*p_List == p_node) {
    *p_List = NEXT(*p_List);  /* Continue with the next. */

  } else {
    node_t* iter_node;
    for (iter_node = *p_List; iter_node != NULL && NEXT(iter_node) != p_node; ) {
      iter_node = NEXT(iter_node);
    }

    if (iter_node == NULL) {
      return FAIL;

    } else {
      NEXT(iter_node) = NEXT(p_node);
    }
  }

  List_free_node(&p_node);
  return SUCC;
}

/* Instead return pointer to pointer it is more convenient to pass out param. */
status
List_delete_head(List_T* p_List, generic_ptr* pp_data__)
{
  Require(*p_List);

  *pp_data__ = DATA(*p_List);
  return List_delete_node(p_List, *p_List /* fist node in practice */);
}

status
List_traverse(List_T self, status (*apply_fn)(generic_ptr))
{
  if (List_is_empty(self))
  { return SUCC; }

  if ((*apply_fn)(DATA(self)) == FAIL) {
    return FAIL;

  } else {
    return List_traverse(NEXT(self), apply_fn);
  }
}

List_T
List_iterator(List_T self, List_T last_return)
{
  return (last_return == NULL) ? self : NEXT(last_return);
}

size_t
List_length(List_T self)
{
  size_t accum = 0;
  node_t* curr_node = NULL;

  while ((curr_node = List_iterator(self, curr_node)) != NULL)
  { ++accum; }

  return accum;
}

status
List_find_key(List_T self, compare_data_FN comp_data_fn, generic_ptr key, node_t** pp_keynode__)
{
  node_t* p_curr = NULL;
  while ((p_curr = List_iterator(self, p_curr)) != NULL) {

    if (comp_data_fn(key, DATA(p_curr)) == true) {
      *pp_keynode__ = p_curr;
      return SUCC;
    }
  }
  return FAIL;
}

/* Recursively delete all connections then free each node's data */
void
List_destroy(List_T* p_List, free_data_FN free_data_fn)
{
  if (List_is_empty(*p_List) == false) {
    List_destroy(&NEXT(*p_List), free_data_fn);

    if (free_data_fn != NULL)
    { free_data_fn(DATA(*p_List)); }

    List_free_node(p_List);
  }
}

void
List_print(const List_T self, print_data_FN print_data_fn)
{
  node_t* curr_node = NULL;
  while ((curr_node = List_iterator(self, curr_node)) != NULL) {
    print_data_fn(DATA(curr_node));
  }
}
