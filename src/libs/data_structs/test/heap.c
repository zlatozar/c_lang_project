#include "data_structs/heap.h"

#include <greatest.h>
#include "test_data.h"

TEST create_add_delete(void)
{
  Heap_T heap = Heap_new();
  ASSERT(Heap_is_empty(heap));

  Heap_insert(heap, Test_elm(42), cmp_fn);
  Heap_insert(heap, Test_elm(41), cmp_fn);
  Heap_insert(heap, Test_elm(40), cmp_fn);

  ASSERT_FALSE(Heap_is_empty(heap));

  Object_T deleted;
  Heap_delete(heap, 0, cmp_fn, &deleted);

  ASSERT_EQ(40, VALUE(deleted));

  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(create_add_delete);
  GREATEST_MAIN_END();
}
