#include <greatest.h>
#include "data_structs/list.h"

/* __________________________________________________________________________ */
/*                                                          Helper functions  */

typedef struct {
  int x;
} data_t;

static generic_ptr
get_next_elm(int elm)
{
  data_t* data = malloc(sizeof(data_t));
  data->x = elm;

  return (generic_ptr)data;
}

static void
print_data_fn(generic_ptr data)
{
  printf("%d ", ((data_t*)data)->x);
}

static status
apply_fn(generic_ptr data)
{
  print_data_fn(data);
  return SUCC;
}

static bool
comp_data_fn(generic_ptr a_data, generic_ptr b_data)
{
  return ((data_t*)a_data)->x == ((data_t*)b_data)->x;
}

/* __________________________________________________________________________ */

TEST insert(void)
{
  List_T list;
  List_init(&list);
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
  List_T list;
  List_init(&list);

  List_insert(&list, get_next_elm(3));
  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  List_print(list, print_data_fn);

  List_destroy(&list, free);
  PASS();
}

TEST delete_head(void)
{
  List_T list;
  List_init(&list);

  List_insert(&list, get_next_elm(3));
  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  /* Common pattern when work with pointer to pointer. */
  data_t* p_deleted;
  List_delete_head(&list, (generic_ptr*) &p_deleted);
  free(p_deleted);

  ASSERT_EQ(2, List_length(list));

  List_destroy(&list, free);
  PASS();
}

TEST append(void)
{
  List_T list;
  List_init(&list);

  List_insert(&list, get_next_elm(2));
  List_insert(&list, get_next_elm(1));

  List_append(&list, get_next_elm(42));
  ASSERT_EQ(3, List_length(list));

  List_destroy(&list, free);
  PASS();
}

TEST traverse(void)
{
  List_T list;
  List_init(&list);

  List_insert(&list, get_next_elm(30));
  List_insert(&list, get_next_elm(20));
  List_insert(&list, get_next_elm(10));

  List_traverse(list, apply_fn);

  List_destroy(&list, free);
  PASS();
}

TEST find_key(void)
{
  List_T list;
  List_init(&list);

  List_append(&list, get_next_elm(1));
  List_append(&list, get_next_elm(2));
  List_append(&list, get_next_elm(3));
  List_append(&list, get_next_elm(42));

  data_t* key = malloc(sizeof(data_t));
  key->x = 42;

  node_t* match_node;

  List_find_key(list, comp_data_fn, (generic_ptr)key, &match_node);
  data_t* p_match_node_data = (data_t*)DATA(match_node);

  ASSERT(p_match_node_data->x == 42);

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
