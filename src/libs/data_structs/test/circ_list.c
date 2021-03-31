#include "data_structs/circ_list.h"

#include <greatest.h>
#include "test_data.h"

TEST insert(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  CircList_insert(&circlist, get_next_elm(1));
  CircList_insert(&circlist, get_next_elm(2));
  CircList_insert(&circlist, get_next_elm(3));

  // ASSERT_EQ(3, List_length(circlist));

  // CircList_destroy(&circlist, free_elm);
  PASS();
}

TEST delete(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  CircList_insert(&circlist, get_next_elm(1));
  CircList_insert(&circlist, get_next_elm(2));
  CircList_insert(&circlist, get_next_elm(3));

  // ASSERT_EQ(3, List_length(circlist));

  Data_T deleted;
  CircList_delete(&circlist, (Generic_T*) &deleted);
  FREE(deleted);

  // ASSERT_EQ(2, List_length(list));

  // CircList_destroy(&circlist, free_elm);
  PASS();
}


GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(insert);
  RUN_TEST(delete);
  GREATEST_MAIN_END();
}
