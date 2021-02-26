#include "data_structs/list.h"

#include "lang/assert.h"
#include "lang/mem.h"

struct node {
  void* datapointer;
  struct node* next;
};

#define DATA(p_node) ((p_node)->datapointer)
#define NEXT(p_node) ((p_node)->next)

/* __________________________________________________________________________ */
/*                                                                    Public  */

void
List_allocate_node(List_T* p_List, void* data)
{
  node_t* p_node;
  NEW(p_node);

  *p_List = p_node;

  DATA(p_node) = data;
  NEXT(p_node) = NULL;
}

void
List_free_node(List_T* p_List)
{
  FREE(*p_List);
}

void
List_init(List_T* p_List)
{
  *p_List = NULL;
}

bool
List_is_empty(List_T List)
{
  return (List == NULL) ? true : false;
}

void
List_insert(List_T* p_List, void* data)
{
  node_t* p_node;
  List_allocate_node(&p_node, data);

  NEXT(p_node) = *p_List;
  *p_List = p_node;
}

void
append(List_T* p_List, void* data)
{
  node_t* p_node;
  List_allocate_node(&p_node, data);

  if (List_is_empty(*p_List) == true) {
    *p_List = p_node;

  } else {
    List_T tmplist;
    for (tmplist = *p_List; NEXT(tmplist) != NULL; tmplist = NEXT(tmplist)) ;

    NEXT(tmplist) = p_node;
  }
}

bool
List_delete_node(List_T* p_List, node_t* p_node)
{
  Require(*p_List);

  if (*p_List == p_node) {
    *p_List = NEXT(*p_List);

  } else {
    List_T tmplist;
    for (tmplist = *p_List; tmplist != NULL \
                   && NEXT(tmplist) != p_node; tmplist = NEXT(tmplist)) ;

    if (tmplist == NULL)
    { return false; }
    else
    { NEXT(tmplist) = NEXT(p_node); }
  }

  List_free_node(&p_node);
  return true;
}

bool
List_delete(List_T* p_List, void* p_data_)
{
  Require(*p_List);

  // TODO: *p_data_ = DATA(*p_List);

  return List_delete_node(p_List, *p_List);
}
