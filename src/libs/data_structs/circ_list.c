#include "data_structs/circ_list.h"

#include "data_structs/list.h"

#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

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
CircList_insert(CircList_T* p_circlist, Generic_T p_data)
{
  List_insert(p_circlist, p_data);
}
