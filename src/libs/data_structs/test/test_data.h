#include "lang/memory.h"
#include "logger/log.h"

typedef struct {
  int value;
} data_t;

typedef data_t* Data_T;

Data_T
Data_new(int initial_value)
{
  Data_T data;
  NEW(data);

  data->value = initial_value;

  return data;
}

Generic_T
get_next_elm(int elm)
{
  return (Generic_T)Data_new(elm);
}

/* Any kind of stored data. */
int
get_node_data_value(void* p_node)
{
  Generic_T* node = (Generic_T*)p_node;
  return ((Data_T)*node)->value;
}

void
print_data_fn(Generic_T data)
{
  printf("-> %d ", ((Data_T)data)->value);
}

bool
apply_fn(Generic_T data)
{
  print_data_fn(data);
  return true;
}

bool
increment_fn(Generic_T data)
{
  ((Data_T)data)->value++;
  return true;
}

bool
comp_data_fn(Generic_T a_data, Generic_T b_data)
{
  return ((Data_T)a_data)->value == ((Data_T)b_data)->value;
}

void
free_elm(void* elm)
{
  FREE(elm);
}

void
extra_free_elm(void* elm)
{
  Log_debug(" Extra work before FREE(%d);", ((Data_T)elm)->value);
  FREE(elm);
}
