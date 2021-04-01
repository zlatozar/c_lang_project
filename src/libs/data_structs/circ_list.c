#include "data_structs/circ_list.h"
/* To reuse functions. Transitional dependencies form `list_rep.h`. */
#include "data_structs/list.h"

#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

/* Delete given `p_node` from `p_circlist`. */
static void
__delete_node(CircList_T* p_circlist, node_t* p_node)
{
  if (p_node == NEXT(p_node)) {
    /* Delete the only node in the list. */
    *p_circlist = NULL;

  } else {
    node_t* node_iter;
    for (node_iter = NEXT(*p_circlist); node_iter != *p_circlist
         && NEXT(node_iter) != p_node; ) {

      node_iter = NEXT(node_iter);
    }

    Ensure(NEXT(node_iter) == p_node);

    NEXT(node_iter) = NEXT(p_node);
    if (p_node == *p_circlist) {
      /* Deleting the last node in the list, so update `p_circlist`. */
      *p_circlist = node_iter;
    }
  }

  List__free_node(&p_node);
}

/* __________________________________________________________________________ */

CircList_T
CircList_new(void)
{
  return List_new();
}

bool
CircList_is_empty(CircList_T circlist)
{
  return List_is_empty(circlist);
}

/* Every time insert after current first element. First element remains fist. */
void
CircList_insert(CircList_T* p_circlist, Generic_T data)
{
  Require(p_circlist);

  node_t* new_node;
  List__allocate_node(&new_node, data);

  if (CircList_is_empty(*p_circlist)) {
    /* Point to itself - circular list with one element. */
    NEXT(new_node) = new_node;
    *p_circlist = new_node;

  } else {

    NEXT(new_node) = NEXT(*p_circlist);
    NEXT(*p_circlist) = new_node;
  }
}

void
CircList_append(CircList_T* p_circlist, Generic_T data)
{
  CircList_insert(p_circlist, data);
  *p_circlist = NEXT(*p_circlist);
}

bool
CircList_delete(CircList_T* p_circlist, Generic_T* p_data__)
{
  if (CircList_is_empty(*p_circlist)) {
    Log_debug("Can't delete from empty list.");
    return false;
  }

  *p_data__ = DATA(NEXT(*p_circlist));
  __delete_node(p_circlist, *p_circlist /* Fist node in practice. */);

  return true;
}

bool
CircList_traverse(CircList_T circlist, bool (*apply_fn)(Generic_T))
{
  node_t* tmp;
  if (CircList_is_empty(circlist)) {
    return true;
  }

  tmp = circlist;
  do {
    /* Start from the bottom. */
    tmp = NEXT(tmp);

    if (!(*apply_fn)(DATA(tmp))) {
      Log_error("Can't apply function.");
      return false;
    }
  } while (tmp != circlist) ;

  return true;
}

node_t*
CircList_nth(CircList_T circlist, int idx)
{
  if (CircList_is_empty(circlist)) {
    return NULL;
  }

  if (idx == -1) {
    return circlist;
  }

  node_t* iter = circlist;
  do {
    iter = NEXT(iter);
    --idx;

  } while (idx > 0 && iter != circlist);

  return (idx != 0) ? NULL : iter;
}

node_t*
CircList_iterator(CircList_T circlist, node_t* last_return)
{
  if (last_return == NULL) {
    return (circlist) ? NEXT(circlist) : NULL;
  }

  /* Do we reach the beginning? */
  return (last_return == circlist) ? NULL : NEXT(last_return);
}

size_t
CircList_length(CircList_T circlist)
{
  size_t accum = 0;
  node_t* curr_node = NULL;

  while ((curr_node = CircList_iterator(circlist, curr_node)) != NULL)
  { ++accum; }

  return accum;
}

void
CircList_destroy(CircList_T* p_circlist, free_data_FN free_data_fn)
{
  Require(p_circlist);

  /* First iteration will initialize it. */
  CircList_T next_tmp = NULL;
  for ( ; *p_circlist != next_tmp; *p_circlist = next_tmp) {
    next_tmp = NEXT(*p_circlist);

    if (free_data_fn != NULL) {
      free_data_fn(DATA(*p_circlist));
    }

    FREE(*p_circlist);
  }
}

void
CircList_free(CircList_T* p_circlist)
{
  CircList_destroy(p_circlist, NULL);
}
