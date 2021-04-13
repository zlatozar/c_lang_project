#include "data_structs/binary_tree.h"

#include <greatest.h>

TEST create_add(void)
{
  ASSERT(1);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(create_add);
  GREATEST_MAIN_END();
}
