#include "data_structs/list.h"

#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

static void
List_allocate_node(node_t** pp_node, Generic_T data)
{
  node_t* p_node;
  NEW(p_node);

  *pp_node = p_node;

  DATA(p_node) = data;
  NEXT(p_node) = NULL;
}

static void
List_free_node(node_t** pp_node)
{
  FREE(*pp_node);
}

/* __________________________________________________________________________ */

List_T
List_new(void)
{
  List_T list = NULL;
  return list;
}

bool
List_is_empty(List_T list)
{
  return (list == NULL) ? true : false;
}

void
List_insert(List_T* p_List, Generic_T data)
{
  Require(p_List);

  node_t* p_node;
  List_allocate_node(&p_node, data);

  NEXT(p_node) = *p_List;
  *p_List = p_node;
}

void
List_append(List_T* p_List, Generic_T data)
{
  node_t* p_node;
  List_allocate_node(&p_node, data);

  if (List_is_empty(*p_List)) {
    *p_List = p_node;

  } else {
    node_t* iter_node;
    for (iter_node = *p_List; NEXT(iter_node) != NULL; ) {
      iter_node = NEXT(iter_node);
    }

    NEXT(iter_node) = p_node;
  }
}

status
List_delete_node(List_T* p_List, node_t* p_node)
{
  if (*p_List == p_node) {
    *p_List = NEXT(*p_List);  /* Continue with the next. */

  } else {
    node_t* iter_node;
    for (iter_node = *p_List; iter_node != NULL && NEXT(iter_node) != p_node; ) {
      iter_node = NEXT(iter_node);
    }

    if (iter_node == NULL) {
      /* Delete from empty list. */
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
List_delete_head(List_T* p_List, Generic_T* p_data__)
{
  if (List_is_empty(*p_List)) {
    Log_debug("Can't delete from empty list");
    return FAIL;
  }

  *p_data__ = DATA(*p_List);
  return List_delete_node(p_List, *p_List /* fist node in practice */);
}

/* Iterate tail recursively. */
status
List_traverse(List_T list, status (*apply_fn)(Generic_T))
{
  Require(apply_fn);

  if (List_is_empty(list))
  { return SUCC; }

  if ((*apply_fn)(DATA(list)) == FAIL) {
    return FAIL;

  } else {
    return List_traverse(NEXT(list), apply_fn);
  }
}

List_T
List_iterator(List_T list, List_T last_return)
{
  return (last_return == NULL) ? list : NEXT(last_return);
}

size_t
List_length(List_T list)
{
  size_t accum = 0;
  node_t* curr_node = NULL;

  while ((curr_node = List_iterator(list, curr_node)) != NULL)
  { ++accum; }

  return accum;
}

/* Searching same as `key` and if found, `pp_keynode__` is instance of it in the list. */
status
List_find_key(List_T list, compare_data_FN comp_data_fn, Generic_T key, node_t** pp_keynode__)
{
  Require(list);
  Require(comp_data_fn);
  Require(key);

  node_t* p_curr = NULL;
  while ((p_curr = List_iterator(list, p_curr)) != NULL) {

    if (comp_data_fn(key, DATA(p_curr))) {
      *pp_keynode__ = p_curr;
      return SUCC;
    }
  }
  return FAIL;
}

void
List_print(const List_T list, print_data_FN print_data_fn)
{
  node_t* curr_node = NULL;
  while ((curr_node = List_iterator(list, curr_node)) != NULL) {
    print_data_fn(DATA(curr_node));
  }
}

/* Recursively delete all connections then free each node's data */
void
List_destroy(List_T* p_List, free_data_FN free_data_fn)
{
  Require(p_List);
  Require(free_data_fn);

  if (List_is_empty(*p_List) == false) {
    List_destroy(&NEXT(*p_List), free_data_fn);

    if (free_data_fn != NULL)
    { free_data_fn(DATA(*p_List)); }

    List_free_node(p_List);
  }
}
