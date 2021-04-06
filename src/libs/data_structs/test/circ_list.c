#include "data_structs/circ_list.h"

#include <greatest.h>
#include "test_data.h"

TEST insert(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  // In memory: 1, 4, 3, 2
  CircList_insert(&circlist, get_next_elm(1));

  CircList_insert(&circlist, get_next_elm(2));
  CircList_insert(&circlist, get_next_elm(3));
  CircList_insert(&circlist, get_next_elm(4));

  ASSERT_EQ(4, CircList_length(circlist));

  CircList_destroy(&circlist, free_elm);
  PASS();
}

TEST append(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  CircList_append(&circlist, get_next_elm(1));
  CircList_append(&circlist, get_next_elm(2));
  CircList_append(&circlist, get_next_elm(3));
  CircList_append(&circlist, get_next_elm(4));

  ASSERT_EQ(4, CircList_length(circlist));
  // Traverse: 1, 2, 3, 4
  CircList_traverse(circlist, apply_fn);

  CircList_destroy(&circlist, free_elm);
  PASS();
}

TEST delete(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  CircList_insert(&circlist, get_next_elm(1));
  CircList_insert(&circlist, get_next_elm(2));
  CircList_insert(&circlist, get_next_elm(3));

  ASSERT_EQ(3, CircList_length(circlist));

  Data_T deleted;
  CircList_delete(&circlist, (Generic_T*) &deleted);
  FREE(deleted);

  ASSERT_EQ(2, CircList_length(circlist));

  CircList_destroy(&circlist, free_elm);
  PASS();
}

TEST traverse(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  CircList_insert(&circlist, get_next_elm(1));
  CircList_insert(&circlist, get_next_elm(2));
  CircList_insert(&circlist, get_next_elm(3));
  CircList_insert(&circlist, get_next_elm(4));

  ASSERT_EQ(4, CircList_length(circlist));

  // Traverse: 4, 3, 2, 1
  CircList_traverse(circlist, apply_fn);

  CircList_destroy(&circlist, free_elm);
  PASS();
}

TEST insert_nth(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  CircList_insert(&circlist, get_next_elm(1));
  CircList_insert(&circlist, get_next_elm(2));
  CircList_insert(&circlist, get_next_elm(3));
  CircList_insert(&circlist, get_next_elm(4));

  ASSERT_EQ(4, CircList_length(circlist));

  /* Count backward, because of `CircList_insert`. */
  node_t* second = CircList_nth(circlist, 2);
  ASSERT_EQ(3, get_node_data_value(second));

  CircList_destroy(&circlist, free_elm);
  PASS();
}

TEST append_nth(void)
{
  CircList_T circlist = CircList_new();
  ASSERT(CircList_is_empty(circlist));

  CircList_append(&circlist, get_next_elm(1));
  CircList_append(&circlist, get_next_elm(2));
  CircList_append(&circlist, get_next_elm(3));
  CircList_append(&circlist, get_next_elm(4));

  ASSERT_EQ(4, CircList_length(circlist));

  /* Count forward, because of `CircList_append`. */
  node_t* second = CircList_nth(circlist, 2);
  ASSERT_EQ(2, get_node_data_value(second));

  CircList_destroy(&circlist, free_elm);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(insert);
  RUN_TEST(append);
  RUN_TEST(delete);
  RUN_TEST(traverse);
  RUN_TEST(insert_nth);
  RUN_TEST(append_nth);
  GREATEST_MAIN_END();
}
