#include "data_structs/binary_tree.h"

#include <greatest.h>
#include "test_data.h"

TEST create_add_delete(void)
{
  BinTree_T btree = BinTree_new();
  BinTree_make_root(&btree, Test_elm(1), NULL, NULL);

  BinTree_free(&btree);
  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(create_add_delete);
  GREATEST_MAIN_END();
}
