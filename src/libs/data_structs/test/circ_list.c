#include "data_structs/circ_list.h"

#include <greatest.h>
#include "test_data.h"

TEST insert(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  /* CircList_insert(&circlist, get_next_elm(1)); */
  /* CircList_insert(&circlist, get_next_elm(2)); */
  /* CircList_insert(&circlist, get_next_elm(3)); */

  /* ASSERT_EQ(3, List_length(list)); */

  /* CircList_destroy(&circlist, free_elm); */
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(insert);
  GREATEST_MAIN_END();
}
