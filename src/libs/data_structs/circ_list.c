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

    Ensure(NEXT(node_iter) != p_node);

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

void
CircList_insert(CircList_T* p_circlist, Generic_T data)
{
  Require(p_circlist);

  node_t* new_node;
  List__allocate_node(&new_node, data);

  if (CircList_is_empty(*p_circlist)) {
    /* Points to itself. */
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
