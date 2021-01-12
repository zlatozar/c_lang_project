#include "deps/test/unity.h"

#include "macros/lang.h"
#include "common/lang/except.h"

#include "common/data_structs/list.h"

void setUp () {}
void tearDown () {}

// _____________________________________________________________________________
//                                                                   Unit Tests

LOCAL void print_str(const void* str) {
  printf("%s ", (char*) str);
}

LOCAL void print_int(const void* integer) {
  printf("%i ", PTR_TO_INT(integer));
}

void test_List_cons() {
  List_T list_str = List_cons("\nHello, ", List_cons("world!", NULL));
  List_print(print_str, list_str);
  printf("\n");

  List_T list_int = List_cons(INT_TO_PTR(1), List_cons(INT_TO_PTR(2), NULL));
  List_print(print_int, list_int);
  printf("\n");
}

void test_List_nth() {
  List_T list_int = List_cons(INT_TO_PTR(1), List_cons(INT_TO_PTR(2), NULL));
  TEST_ASSERT_EQUAL(2, PTR_TO_INT( List_nth(list_int, 1)));

  TRY
      printf("nth: %d\n", PTR_TO_INT( List_nth(list_int, 2)));

  ELSE
      printf("Expected out of bound exception\n");
  END_TRY;
}

void test_List_free() {
  List_T list_int = List_cons(INT_TO_PTR(1),
                              List_cons(INT_TO_PTR(2),
                                        List_cons(INT_TO_PTR(3), NULL)));

  TEST_ASSERT_EQUAL(3, List_length(list_int));
  List_free(&list_int);
  TEST_ASSERT_EQUAL(0, List_length(list_int));
}

void test_List_drop() {
  List_T list_int = List_cons(INT_TO_PTR(1),
                              List_cons(INT_TO_PTR(2),
                                        List_cons(INT_TO_PTR(3), NULL)));

  TEST_ASSERT_EQUAL(3, List_length(list_int));
  List_drop(list_int, 1);
  TEST_ASSERT_EQUAL(2, List_length(list_int));
}

// _____________________________________________________________________________
//                                                                   Test Suite

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_List_cons);
  RUN_TEST(test_List_nth);
  RUN_TEST(test_List_free);
  RUN_TEST(test_List_drop);

  return UNITY_END();
}
