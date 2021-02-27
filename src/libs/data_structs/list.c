#include "data_structs/list.h"

#include <stdlib.h>       /* malloc, free */
#include "lang/assert.h"
#include "logger/log.h"

struct node {
  generic_ptr datapointer;
  struct node* next;
};

#define DATA(p_node) ((p_node)->datapointer)
#define NEXT(p_node) ((p_node)->next)

/* __________________________________________________________________________ */
/*                                                                    Public  */

mem_status
List_allocate_node(List_T* p_List, generic_ptr data)
{
  node_t* p_node = (node_t*) malloc(sizeof(p_node));
  if (p_node == NULL) {
    Log_error("Root cause - 'malloc' do not succeed.");
    return ERROR;
  }

  *p_List = p_node;

  DATA(p_node) = data;
  NEXT(p_node) = NULL;

  return OK;
}

void
List_free_node(List_T* p_List)
{
  free(*p_List);
  *p_List = NULL;
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

mem_status
List_insert(List_T* p_List, generic_ptr data)
{
  node_t* p_node;
  if (List_allocate_node(&p_node, data) == ERROR) {
    Log_error("Can't insert node.");
    return ERROR;
  }

  NEXT(p_node) = *p_List;
  *p_List = p_node;

  return OK;
}

mem_status
append(List_T* p_List, generic_ptr data)
{
  node_t* p_node;

  if (List_allocate_node(&p_node, data) == ERROR) {
    Log_error("Can't append to list.");
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

bool
List_delete_node(List_T* p_List, node_t* p_node)
{
  Require(*p_List);

  if (*p_List == p_node) {
    *p_List = NEXT(*p_List);

  } else {
    node_t* iter_node;
    for (iter_node = *p_List; iter_node != NULL && NEXT(iter_node) != p_node; ) {
      iter_node = NEXT(iter_node);
    }

    if (iter_node == NULL) {
      return false;

    } else {
      NEXT(iter_node) = NEXT(p_node);
    }
  }

  List_free_node(&p_node);
  return true;
}

bool
List_delete(List_T* p_List, generic_ptr* p_data__)
{
  Require(*p_List);

  *p_data__ = DATA(*p_List);

  return List_delete_node(p_List, *p_List);
}
