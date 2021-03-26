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

void
print_data_fn(Generic_T data)
{
  printf("%d ", ((Data_T)data)->value);
}

status
apply_fn(Generic_T data)
{
  print_data_fn(data);
  return SUCC;
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
