#include "polymorphic/interface.h"

#include <stdio.h>
#include "lang/memory.h"

/* Implementation one specific. */
struct impl_one {
  int a;
};

typedef struct impl_one* ImplOne_T;

void*
ImplOne_new()
{
  ImplOne_T impl;
  NEW(impl);
  impl->a = 1;

  printf("\nFirst implementation - `ImplOne_T` was created!\n");
  return (void*)impl;
}

int
ImplOne_add(void* interface_impl, int a, int b)
{
  ImplOne_T impl = interface_impl;

  int result = impl->a + a + b;
  printf("result = %d\n", result);

  return result;
}

void
ImplOne_destroy(void* interface_impl)
{
  FREE(interface_impl);
  printf("First implementation - `ImplOne` was destroyed...\n\n");
}

// "Interface" first implementation

interface_t impl_one = {
  ImplOne_new,
  ImplOne_add,
  ImplOne_destroy
};
