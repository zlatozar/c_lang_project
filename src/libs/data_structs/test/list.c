#include <greatest.h>
#include "data_structs/list.h"

/* __________________________________________________________________________ */
/*                                                          Helper functions  */

typedef struct {
  int x;
} data_t;

typedef data_t* Data_T;

static Generic_T
get_next_elm(int elm)
{
  Data_T data = malloc(sizeof(*data));
  data->x = elm;

  return (Generic_T)data;
}

static void
print_data_fn(Generic_T data)
{
  printf("%d ", ((Data_T)data)->x);
}

static status
apply_fn(Generic_T data)
{
  print_data_fn(data);
  return SUCC;
}

static bool
comp_data_fn(Generic_T a_data, Generic_T b_data)
{
  return ((Data_T)a_data)->x == ((Data_T)b_data)->x;
}

/* __________________________________________________________________________ */

TEST insert(void)
{
  List_T list = List_new();
  ASSERT(List_is_empty(list));

  List_insert(&list, get_next_elm(1));
  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(3));

  ASSERT_EQ(3, List_length(list));

  List_destroy(&list, free);
  PASS();
}

TEST print(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(3));
  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  List_print(list, print_data_fn);

  List_destroy(&list, free);
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
  free(deleted);

  ASSERT_EQ(2, List_length(list));

  List_destroy(&list, free);
  PASS();
}

TEST append(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  List_append(&list, get_next_elm(42));
  ASSERT_EQ(3, List_length(list));

  List_destroy(&list, free);
  PASS();
}

TEST traverse(void)
{
  List_T list = List_new();

  List_insert(&list, get_next_elm(30));
  List_insert(&list, get_next_elm(20));
  List_insert(&list, get_next_elm(10));

  status traversed = List_traverse(list, apply_fn);
  ASSERT(traversed == SUCC);

  List_destroy(&list, free);
  PASS();
}

TEST find_key(void)
{
  List_T list = List_new();

  List_append(&list, get_next_elm(1));
  List_append(&list, get_next_elm(2));
  List_append(&list, get_next_elm(3));
  List_append(&list, get_next_elm(42));

  Data_T key = malloc(sizeof(data_t));
  key->x = 42;

  /* Contains found node. */
  node_t* match_node;

  status found = List_find_key(list, comp_data_fn, (Generic_T)key, &match_node);
  ASSERT(found == SUCC);

  Data_T match_node_data = (Data_T)DATA(match_node);
  ASSERT(match_node_data->x == 42);

  List_destroy(&list, free);
  free(key);
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
  GREATEST_MAIN_END();
}
