#include <greatest.h>
#include "lang/memory.h"

typedef struct {
  int a;
  int b;
} test_t;

typedef test_t* Test_T;

TEST allocate_macro(void)
{
  Test_T test;

  NEW(test);
  FREE(test);

  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(allocate_macro);
  GREATEST_MAIN_END();
}
