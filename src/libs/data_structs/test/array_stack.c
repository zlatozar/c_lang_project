#include "data_structs/stack.h"

#include <greatest.h>
#include "test_data.h"

TEST destroy_empty(void)
{
  Stack_T stack = Stack_new(1);
  ASSERT(Stack_is_empty(stack));

  Stack_push(stack, Test_elm(1));
  ASSERT_FALSE(Stack_is_empty(stack));

  Data_T elm;
  ASSERT(Stack_pop(stack, (Generic_T*) &elm));
  ASSERT(elm);
  FREE(elm);

  ASSERT(Stack_is_empty(stack));

  Stack_destroy(stack, free);
  PASS();
}

TEST destroy_non_empty(void)
{
  Stack_T stack = Stack_new_def();
  ASSERT(Stack_is_empty(stack));

  Stack_push(stack, Test_elm(3));
  Stack_push(stack, Test_elm(2));
  Stack_push(stack, Test_elm(1));
  ASSERT_FALSE(Stack_is_empty(stack));

  Stack_destroy(stack, free_elm_fn);
  PASS();
}

TEST free_non_empty(void)
{
  Stack_T stack = Stack_new(3);
  ASSERT(Stack_is_empty(stack));

  Stack_push(stack, Test_elm(3));
  Stack_push(stack, Test_elm(2));
  Stack_push(stack, Test_elm(1));
  ASSERT_FALSE(Stack_is_empty(stack));

  Stack_free(stack);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(destroy_empty);
  RUN_TEST(destroy_non_empty);
  RUN_TEST(free_non_empty);
  GREATEST_MAIN_END();
}
