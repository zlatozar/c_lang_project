#include "data_structs/stack.h"

#include <greatest.h>
#include "test_data.h"

TEST free_empty(void)
{
  Stack_T stack = Stack_new();
  ASSERT(Stack_is_empty(stack));

  Stack_push(stack, get_next_elm(1));
  ASSERT_FALSE(Stack_is_empty(stack));

  Data_T elm;
  Stack_pop(stack, (Generic_T*) &elm);
  FREE(elm);

  ASSERT(Stack_is_empty(stack));

  Stack_destroy(stack, free_elm);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(free_empty);
  GREATEST_MAIN_END();
}
