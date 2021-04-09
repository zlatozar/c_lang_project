#include "data_structs/sparse_matrix.h"

#include <greatest.h>

TEST insert(void)
{
  ASSERT(1);

  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(insert);
  GREATEST_MAIN_END();
}
