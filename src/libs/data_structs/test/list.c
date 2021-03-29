#include "data_structs/list.h"

#include <greatest.h>
#include "test_data.h"

TEST insert(void)
{
  List_T list = List_new();
  ASSERT(List_is_empty(list));

  List_insert(&list, get_next_elm(1));
  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(3));

  ASSERT_EQ(3, List_length(list));

  List_destroy(&list, free_elm);
  PASS();
}

TEST print(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(3));
  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  List_print(list, print_data_fn);

  List_destroy(&list, free_elm);
  PASS();
}

TEST delete_head(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(3));
  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  /* Common pattern when work with pointer to pointer. */
  Data_T deleted;
  List_delete_head(&list, (Generic_T*) &deleted);
  FREE(deleted);

  ASSERT_EQ(2, List_length(list));

  List_destroy(&list, free_elm);
  PASS();
}

TEST append(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  List_append(&list, get_next_elm(42));
  ASSERT_EQ(3, List_length(list));

  List_destroy(&list, free_elm);
  PASS();
}

TEST traverse(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(30));
  List_insert(&list, get_next_elm(20));
  List_insert(&list, get_next_elm(10));
  ASSERT_EQ(3, List_length(list));

  ASSERT(List_traverse(list, apply_fn));

  List_destroy(&list, free_elm);
  PASS();
}

TEST find_key(void)
{
  List_T list = List_new();

  List_append(&list, get_next_elm(1));
  List_append(&list, get_next_elm(2));
  List_append(&list, get_next_elm(3));
  List_append(&list, get_next_elm(42));

  /* Use malloc/free and NEW etc/FREE consistently. */

  Data_T key = malloc(sizeof(data_t));
  key->value = 42;

  /* Contains found node. */
  node_t* match_node;

  ASSERT(List_find_key(list, comp_data_fn, (Generic_T)key, &match_node));

  Data_T match_node_data = (Data_T)DATA(match_node);
  ASSERT(match_node_data->value == 42);

  List_destroy(&list, free_elm);
  free(key);

  PASS();
}

TEST length(void)
{
  List_T list = List_new();

  ASSERT_EQ(0, List_length(list));

  List_destroy(&list, free_elm);
  PASS();
}

TEST free_non_empty(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  List_free(&list);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(insert);
  RUN_TEST(print);
  RUN_TEST(delete_head);
  RUN_TEST(append);
  RUN_TEST(traverse);
  RUN_TEST(find_key);
  RUN_TEST(length);
  RUN_TEST(free_non_empty);
  GREATEST_MAIN_END();
}
