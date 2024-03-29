#include "lang/memory.h"
#include "logger/log.h"

typedef struct {
  int value;
} data_t;

typedef data_t* Data_T;

#define VALUE(data)  (((Data_T)data)->value)

Data_T
Data_new(int initial_value)
{
  Data_T data;
  NEW(data);

  data->value = initial_value;

  return data;
}

Object_T
Test_elm(int elm)
{
  return (Object_T)Data_new(elm);
}

/* Any kind of stored data. */
int
Test_value(void* p_node)
{
  Object_T* node = (Object_T*)p_node;
  return VALUE(*node);
}

void
print_fn(Object_T data)
{
  printf("-> %d ", VALUE(data));
}

bool
apply_fn(Object_T data)
{
  print_fn(data);
  return true;
}

bool
inc_value_fn(Object_T data)
{
  ((Data_T)data)->value++;
  return true;
}

bool
equal_fn(Object_T a_data, Object_T b_data)
{
  return VALUE(a_data) == VALUE(b_data);
}

int
cmp_fn(Object_T a_data, Object_T b_data)
{
  int a = VALUE(a_data);
  int b = VALUE(b_data);

  /* Effective way to compare two integers. */
  return (a > b) - (a < b);
}

void
free_elm_fn(void* elm)
{
  FREE(elm);
}

void
extra_free_elm_fn(void* elm)
{
  Log_debug(" Extra work before FREE(%d);", VALUE(elm));
  FREE(elm);
}
