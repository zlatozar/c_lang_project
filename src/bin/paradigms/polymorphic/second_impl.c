#include "polymorphic/interface.h"

#include <stdio.h>
#include "lang/memory.h"

/* Implementation two specific. */
struct impl_two {
  int a;
};

typedef struct impl_two* ImplTwo_T;

void*
ImplTwo_new()
{
  ImplTwo_T impl;
  NEW(impl);
  impl->a = 2;

  printf("Second implementation - `ImplTwo_T` was created!\n");
  return (void*)impl;
}

int
ImplTwo_add(void* interface_impl, int a, int b)
{
  ImplTwo_T impl = interface_impl;
  int result = impl->a +  a + b;
  printf("result = %d\n", result);

  return result;
}

void
ImplTwo_destroy(void* interface_impl)
{
  FREE(interface_impl);
  printf("Second implementation - `ImplTwo_T` was destroyed...\n\n");
}

// "Interface" second implementation

interface_t impl_two = {
  ImplTwo_new,
  ImplTwo_add,
  ImplTwo_destroy
};
