#include "data_structs/list_rep.h"

#include "lang/memory.h"

/* `pp_node` points position where node should be situated. */
void
List__allocate_node(node_t** pp_node, Generic_T data)
{
  node_t* p_node;
  NEW(p_node);

  *pp_node = p_node;

  DATA(p_node) = data;
  NEXT(p_node) = NULL;
}

void
List__free_node(node_t** pp_node)
{
  FREE(*pp_node);
}
