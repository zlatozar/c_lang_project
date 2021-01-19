#include "deps/test/unity.h"
#include "macros/lang.h"

#include "common/data_structs/stack.h"

// _____________________________________________________________________________
//                                                               setUp/tearDown

Stack_T stack;

void setUp() {
  stack = Stack_new();
}

void tearDown() {
  Stack_free(&stack);
}

// _____________________________________________________________________________
//                                                                   Unit Tests

void test_Stack_add() {
  Stack_push(stack, INT_TO_PTR(1));
  Stack_pop(stack);
  TEST_ASSERT_TRUE(Stack_isEmpty(stack));
}

// _____________________________________________________________________________
//                                                                   Test Suite

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_Stack_add);

  return UNITY_END();
}
