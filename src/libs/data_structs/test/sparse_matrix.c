#include "data_structs/sparse_matrix.h"

#include <greatest.h>
#include "lang/memory.h"

static void
matrix_put_int(Matrix_T* p_matrix, int row, int col, int value)
{
   int* p_i;

   /* No place for zeros in sparse matrix. */
   if (value == 0) {
     if (Matrix_get(p_matrix, row, col, (Object_T*) &p_i)) {
       Matrix_clear(p_matrix, row, col);
       FREE(p_i);
     }
   }

   p_i = ALLOC(sizeof(*p_i));
   *p_i = value;

   Matrix_put(p_matrix, row, col, (Object_T) p_i);
}

static void
matrix_get_int(Matrix_T* p_matrix, int row, int col, int* p_value__)
{
   int *p_i;
   if (!Matrix_get(p_matrix, row, col, (Object_T*) &p_i))
     *p_value__ = 0;
   else
     *p_value__ = *p_i;
}

/* ______________________________________________________________________________ */

TEST simple_test(void)
{
  Matrix_T matrix = Matrix_new();

  int value = 42;
  matrix_put_int(&matrix, 5, 5, value);

  int saved;
  matrix_get_int(&matrix, 5, 5, &saved);

  ASSERT_EQ(value, saved);

  PASS();
}

GREATEST_MAIN_DEFS();
int main(int argc, char** argv)
{
  GREATEST_MAIN_BEGIN();
  RUN_TEST(simple_test);
  GREATEST_MAIN_END();
}
