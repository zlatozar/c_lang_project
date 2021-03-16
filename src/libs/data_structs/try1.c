#include "data_structs/try.h"

#include <stdio.h>
#include "lang/mem.h"

struct try1 {
  int a;
};

typedef struct try1* Try1_T;

void*
Try1_new()
{
  Try1_T try_impl;
  NEW(try_impl);
  try_impl->a = 1;

  printf("ITry implementation - `try1` was created!\n");
  return (void*)try_impl;
}

int
Try1_add(void* itry, int a, int b)
{
  Try1_T try = itry;

  int result = try->a + a + b;
  printf("result = %d\n", result);

  return result;
}

void
Try1_destroy(void* itry)
{
  FREE(itry);
  printf("ITry implementation - `try1` was destroyed...\n\n");
}

// "Interface" implementation

itry_t try1 = {
  Try1_new,
  Try1_add,
  Try1_destroy
};
