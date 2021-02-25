#include "lang/macros.h"
#include <greatest.h>

TEST bitsizeof_macro(void)
{
  ASSERT(bitsizeof(int));
  PASS();
}

TEST arraysizeof_macro(void)
{
  int a[] = {0, 1, 2};
  ASSERT(ARRAY_SIZE(a) == 3);
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite)
{
  RUN_TEST(bitsizeof_macro);
  RUN_TEST(arraysizeof_macro);
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(the_suite);
  GREATEST_MAIN_END();
}
