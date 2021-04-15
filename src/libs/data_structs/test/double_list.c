#include "data_structs/double_list.h"

#include <greatest.h>
#include "test_data.h"

TEST insert(void)
{
  DoubleList_T list = DoubleList_new();
  ASSERT(DoubleList_is_empty(list));

  DoubleList_insert(&list, Test_elm(1));
  DoubleList_insert(&list, Test_elm(2));
  DoubleList_insert(&list, Test_elm(3));

  ASSERT_EQ(3, DoubleList_length(list));

  DoubleList_destroy(&list, free_elm_fn);
  PASS();
}

TEST append(void)
{
  DoubleList_T list = DoubleList_new();
  ASSERT(DoubleList_is_empty(list));

  DoubleList_append(&list, Test_elm(1));
  DoubleList_append(&list, Test_elm(2));
  DoubleList_append(&list, Test_elm(3));

  ASSERT_EQ(3, DoubleList_length(list));

  DoubleList_destroy(&list, free_elm_fn);
  PASS();
}

TEST delete_head(void)
{
  DoubleList_T list = DoubleList_new();
  ASSERT(DoubleList_is_empty(list));

  DoubleList_insert(&list, Test_elm(1));
  DoubleList_insert(&list, Test_elm(2));
  DoubleList_insert(&list, Test_elm(3));

  ASSERT_EQ(3, DoubleList_length(list));

  Data_T deleted;
  DoubleList_delete_head(&list, (Generic_T*) &deleted);
  FREE(deleted);

  ASSERT_EQ(2, DoubleList_length(list));

  DoubleList_destroy(&list, free_elm_fn);
  PASS();
}

TEST traverse(void)
{
  DoubleList_T list = DoubleList_new();
  ASSERT(DoubleList_is_empty(list));

  DoubleList_insert(&list, Test_elm(4));
  DoubleList_insert(&list, Test_elm(3));
  DoubleList_insert(&list, Test_elm(2));
  DoubleList_insert(&list, Test_elm(1));

  ASSERT_EQ(4, DoubleList_length(list));

  // FIFO(bottom up): 1, 2, 3, 4
  DoubleList_traverse(list, apply_fn);

  DoubleList_destroy(&list, free_elm_fn);
  PASS();
}

TEST insert_nth(void)
{
  DoubleList_T list = DoubleList_new();
  ASSERT(DoubleList_is_empty(list));

  /* Representation: 1, 2, 3, 4 */
  DoubleList_insert(&list, Test_elm(4));
  DoubleList_insert(&list, Test_elm(3));
  DoubleList_insert(&list, Test_elm(2));
  DoubleList_insert(&list, Test_elm(1));

  ASSERT_EQ(4, DoubleList_length(list));

  doublenode_t* second = DoubleList_nth(list, 2);
  ASSERT_EQ(2, Test_value(second));

  doublenode_t* third = DoubleList_nth(list, 3);
  ASSERT_EQ(3, Test_value(third));

  DoubleList_destroy(&list, free_elm_fn);
  PASS();
}

TEST insert_nth_negative(void)
{
  DoubleList_T list = DoubleList_new();
  ASSERT(DoubleList_is_empty(list));

  /* Representation: 1, 2, 3, 4 */
  DoubleList_insert(&list, Test_elm(4));
  DoubleList_insert(&list, Test_elm(3));
  DoubleList_insert(&list, Test_elm(2));
  DoubleList_insert(&list, Test_elm(1));

  ASSERT_EQ(4, DoubleList_length(list));

  doublenode_t* last = DoubleList_nth(list, -1);
  ASSERT_EQ(4, Test_value(last));

  doublenode_t* before_last = DoubleList_nth(list, -2);
  ASSERT_EQ(3, Test_value(before_last));

  DoubleList_destroy(&list, free_elm_fn);
  PASS();
}

TEST cut_paste(void)
{
  DoubleList_T list = DoubleList_new();
  ASSERT(DoubleList_is_empty(list));

  // [1, (2, 3,) 4] -> (42, 2, 3) - [1, 42, 2, 3, 4]
  DoubleList_insert(&list, Test_elm(4));
  DoubleList_insert(&list, Test_elm(3));
  DoubleList_insert(&list, Test_elm(2));
  DoubleList_insert(&list, Test_elm(1));

  ASSERT_EQ(4, DoubleList_length(list));

  doublenode_t* second = DoubleList_nth(list, 2);
  ASSERT_EQ(2, Test_value(second));

  doublenode_t* third = DoubleList_nth(list, 3);
  ASSERT_EQ(3, Test_value(third));

  /* Take inner two. */
  DoubleList_cut(&list, &second, &third);
  ASSERT_EQ(2, DoubleList_length(second));

  DoubleList_insert(&second, Test_elm(42));
  DoubleList_paste(&list, &second);

  DoubleList_traverse(list, apply_fn);

  DoubleList_destroy(&list, free_elm_fn);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(insert);
  RUN_TEST(append);
  RUN_TEST(delete_head);
  RUN_TEST(traverse);
  RUN_TEST(insert_nth);
  RUN_TEST(insert_nth_negative);
  RUN_TEST(cut_paste);
  GREATEST_MAIN_END();
}
