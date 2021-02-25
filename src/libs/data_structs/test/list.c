#include <greatest.h>
#include "data_structs/list.h"

TEST x_should_work(void)
{
  ASSERT_EQ(1, 1);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(x_should_work);
  GREATEST_MAIN_END();
}
