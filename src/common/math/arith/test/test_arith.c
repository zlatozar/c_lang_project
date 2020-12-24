static char rcsid[] = "$Id$";

#include "deps/test/unity.h"
#include "common/math/arith.h"


void setUp () {
}

void tearDown () {
}

// _____________________________________________________________________________
//                                                                   Unit Tests

void test_Arith_max() {
  TEST_ASSERT_EQUAL(5, MAX(5, 2));
  TEST_ASSERT_EQUAL(5, MAX(2, 5));
  TEST_ASSERT_EQUAL(4, MAX(-2, 4));
  TEST_ASSERT_EQUAL(4, MAX(4, -2));
}

void test_Arith_min() {
  TEST_ASSERT_EQUAL(2, MIN(5, 2));
  TEST_ASSERT_EQUAL(-2, MIN(-2, 4));
  TEST_ASSERT_EQUAL(-2, MIN(4, -2));
  TEST_ASSERT_EQUAL(-7, MIN(-7, -2));
  TEST_ASSERT_EQUAL(-7, MIN(-2, -7));
}

void test_Arith_div() {
  TEST_ASSERT_EQUAL(2, Arith_div(13, 5));
  TEST_ASSERT_EQUAL(-3, Arith_div(-13, 5));
  TEST_ASSERT_EQUAL(0, Arith_div(0, 3));
  TEST_ASSERT_EQUAL(12, Arith_div(12, 1));
  TEST_ASSERT_EQUAL(1, Arith_div(12, 12));
  TEST_ASSERT_EQUAL(3, Arith_div(-7, -2));
}

void test_Arith_mod() {
  TEST_ASSERT_EQUAL(3, Arith_mod(13, 5));
  TEST_ASSERT_EQUAL(2, Arith_mod(-13, 5));
  TEST_ASSERT_EQUAL(1, Arith_mod(1, 5));;
  TEST_ASSERT_EQUAL(0, Arith_mod(5, 1));
  TEST_ASSERT_EQUAL(-3, Arith_mod(-13, -5));
}

void test_Arith_floor() {
  TEST_ASSERT_EQUAL(2, Arith_floor(13, 5));
  TEST_ASSERT_EQUAL(-3, Arith_floor(-13, 5));
  TEST_ASSERT_EQUAL(0, Arith_floor(0, 3));
  TEST_ASSERT_EQUAL(12, Arith_floor(12, 1));
  TEST_ASSERT_EQUAL(1, Arith_floor(12, 12));
  TEST_ASSERT_EQUAL(3, Arith_floor(-7, -2));
}

void test_Arith_ceiling() {
  TEST_ASSERT_EQUAL(3, Arith_ceiling(13, 5));
  TEST_ASSERT_EQUAL(-2, Arith_ceiling(-13, 5));

  TEST_ASSERT_EQUAL(4, Arith_ceiling(-7, -2));
}

// _____________________________________________________________________________
//                                                                   Test Suite

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_Arith_max);
  RUN_TEST(test_Arith_min);
  RUN_TEST(test_Arith_div);
  RUN_TEST(test_Arith_mod);
  RUN_TEST(test_Arith_floor);
  RUN_TEST(test_Arith_ceiling);

  return UNITY_END();
}
