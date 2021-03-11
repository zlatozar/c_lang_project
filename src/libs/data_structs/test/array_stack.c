#include <greatest.h>
#include "data_structs/stack.h"

/* __________________________________________________________________________ */
/*                                                          Helper functions  */

typedef struct {
  int x;
} data_t;

typedef data_t* Data_T;

static Generic_T
get_next_elm(int elm)
{
  Data_T data = malloc(sizeof(*data));
  data->x = elm;

  return (Generic_T)data;
}

static void
free_elm(void* elm)
{
  printf("Free %d\n", ((Data_T)elm)->x);
  free(elm);
}

/* __________________________________________________________________________ */

TEST free_empty(void)
{
  Stack_T stack = Stack_new();
  ASSERT(Stack_is_empty(stack));

  Stack_push(stack, get_next_elm(1));
  ASSERT_FALSE(Stack_is_empty(stack));

  Data_T elm;
  Stack_pop(stack, (Generic_T*) &elm);
  free(elm);

  ASSERT(Stack_is_empty(stack));

  Stack_destroy(stack, free);
  PASS();
}

TEST free_not_empty(void)
{
  Stack_T stack = Stack_new();
  ASSERT(Stack_is_empty(stack));

  Stack_push(stack, get_next_elm(3));
  Stack_push(stack, get_next_elm(2));
  Stack_push(stack, get_next_elm(1));
  ASSERT_FALSE(Stack_is_empty(stack));

  Stack_destroy(stack, free_elm);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(free_empty);
  RUN_TEST(free_not_empty);
  GREATEST_MAIN_END();
}
