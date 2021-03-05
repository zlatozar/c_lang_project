#include <greatest.h>
#include "lang/macros.h"

TEST arraysizeof_macro(void)
{
  int a[] = {0, 1, 2};
  ASSERT(ARRAY_SIZE(a) == 3);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(arraysizeof_macro);
  GREATEST_MAIN_END();
}
