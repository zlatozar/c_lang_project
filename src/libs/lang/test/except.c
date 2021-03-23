#include <greatest.h>
#include "lang/except.h"

const Except_T UnitTest_Exception = { "Exception in unit test." };

const Except_T Unknown_Exception = { "Unknown exception." };

/* __________________________________________________________________________ */
/*                                                          Helper functions  */

static void
inner_inner_fn(void) {
  THROW(UnitTest_Exception);
}

static void
inner_fn(void) {
  inner_inner_fn();
}

static void
throw_excpetion(void) {
  inner_fn();
}

/* __________________________________________________________________________ */

TEST throw_catch(void)
{
  TRY
    throw_excpetion();

  CATCH(Unknown_Exception)
    ASSERT(0);
    printf("Un-expected exception.\n");
  CATCH(UnitTest_Exception)
    ASSERT(1);
    printf("-> Expected exception.\n");
  END_TRY;

  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(throw_catch);
  GREATEST_MAIN_END();
}
