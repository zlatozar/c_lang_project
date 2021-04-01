#include "data_structs/list.h"

#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

/* Delete given `p_node` from `p_list`. */
static void
__delete_node(List_T* p_list, node_t* p_node)
{
  if (*p_list == p_node) {
    *p_list = NEXT(*p_list);  /* Continue with the next. */

  } else {
    node_t* iter_node;
    for (iter_node = *p_list; iter_node != NULL && NEXT(iter_node) != p_node; ) {
      iter_node = NEXT(iter_node);
    }

    Ensure(iter_node);

    NEXT(iter_node) = NEXT(p_node);
  }

  List__free_node(&p_node);
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
  return list == NULL;
}

void
List_insert(List_T* p_list, Generic_T data)
{
  Require(p_list);

  node_t* p_node;
  List__allocate_node(&p_node, data);

  NEXT(p_node) = *p_list;
  *p_list = p_node;
}

void
List_append(List_T* p_list, Generic_T data)
{
  node_t* p_node;
  List__allocate_node(&p_node, data);

  if (List_is_empty(*p_list)) {
    *p_list = p_node;

  } else {
    node_t* iter_node;
    for (iter_node = *p_list; NEXT(iter_node) != NULL; ) {
      iter_node = NEXT(iter_node);
    }

    NEXT(iter_node) = p_node;
  }
}

/* Instead return pointer to pointer it is more convenient to pass out param. */
bool
List_delete_head(List_T* p_list, Generic_T* p_data__)
{
  if (List_is_empty(*p_list)) {
    Log_debug("Can't delete from empty list.");
    return false;
  }

  *p_data__ = DATA(*p_list);
  __delete_node(p_list, *p_list /* Fist node in practice. */);

  return true;
}

/* Iterate tail recursively. */
bool
List_traverse(List_T list, bool (*apply_fn)(Generic_T))
{
  Require(apply_fn);

  if (List_is_empty(list))
  { return true; }

  if (!(*apply_fn)(DATA(list))) {
    Log_error("Can't apply function.");
    return false;

  } else {
    return List_traverse(NEXT(list), apply_fn);
  }
}

List_T
List_iterator(List_T list, node_t* last_return)
{
  return (last_return == NULL) ? list : NEXT(last_return);
}

size_t
List_length(List_T list)
{
  size_t n;
  for (n = 0; list != NULL; list = NEXT(list))
  { n++; }

  return n;
}

/* Searching same as `key` and if found, `pp_keynode__` is instance of it in the list. */
bool
List_find_key(List_T list, compare_data_FN comp_data_fn, Generic_T key, node_t** pp_keynode__)
{
  Require(list);
  Require(comp_data_fn);
  Require(key);

  node_t* p_curr = NULL;
  while ((p_curr = List_iterator(list, p_curr)) != NULL) {

    if (comp_data_fn(key, DATA(p_curr))) {
      *pp_keynode__ = p_curr;
      return true;
    }
  }
  return false;
}

void
List_print(const List_T list, print_data_FN print_data_fn)
{
  node_t* curr_node = NULL;
  while ((curr_node = List_iterator(list, curr_node)) != NULL) {
    print_data_fn(DATA(curr_node));
  }
}

void
List_destroy(List_T* p_list, free_data_FN free_data_fn)
{
  Require(p_list);

  /* First iteration will initialize it. */
  List_T next_tmp;
  for ( ; *p_list != NULL; *p_list = next_tmp) {
    next_tmp = NEXT(*p_list);

    if (free_data_fn != NULL) {
      free_data_fn(DATA(*p_list));
    }

    FREE(*p_list);
  }
}

void
List_free(List_T* p_list)
{
  List_destroy(p_list, NULL);
}
