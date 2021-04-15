#include "data_structs/list_stack.h"

#include <greatest.h>
#include "test_data.h"

TEST free_empty(void)
{
  LStack_T stack = LStack_new();
  ASSERT(LStack_is_empty(stack));

  LStack_push(stack, Test_elm(1));
  ASSERT_FALSE(LStack_is_empty(stack));

  Data_T elm;
  LStack_pop(stack, (Generic_T*) &elm);
  FREE(elm);

  ASSERT(LStack_is_empty(stack));

  LStack_destroy(stack, free_elm_fn);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(free_empty);
  GREATEST_MAIN_END();
}
