#include <greatest.h>

#include "data_structs/try.h"
#include "data_structs/try_impl_1.h"
#include "data_structs/try_impl_2.h"

static void
caluclate(itry_t* itry)
{
  void* impl = itry->ITry_new();
  itry->ITry_add(impl, 1, 2);
  itry->ITry_destroy(impl);
}

TEST try_try(void)
{
  caluclate(&try1);
  caluclate(&try2);

  ASSERT(1);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(try_try);
  GREATEST_MAIN_END();
}
