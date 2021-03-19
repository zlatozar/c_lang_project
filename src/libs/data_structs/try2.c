#include "data_structs/try.h"

#include <stdio.h>
#include "lang/memory.h"

struct try2 {
  int a;
};

typedef struct try2* Try2_T;

void*
Try2_new()
{
  Try2_T try_impl;
  NEW(try_impl);
  try_impl->a = 2;

  printf("ITry implementation - `try2` was created!\n");
  return (void*)try_impl;
}

int
Try2_add(void* itry, int a, int b)
{
  Try2_T try = itry;
  int result = try->a +  a + b;
  printf("result = %d\n", result);

  return result;
}

void
Try2_destroy(void* itry)
{
  FREE(itry);
  printf("ITry implementation - `try2` was destroyed...\n\n");
}

// "Interface" implementation

itry_t try2 = {
  Try2_new,
  Try2_add,
  Try2_destroy
};
