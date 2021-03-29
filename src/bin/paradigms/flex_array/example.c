/**
 * @file     example.c
 * @brief    Incomplete array type illustration.
 */
#include <stdlib.h> /* malloc, free */
#include <stdio.h>  /* printf       */

/*
 * ATTENTION:
 * When computing the size of a struct containing a flexible array member
 * using the `sizeof` operator, the flexible array member is ignored.
 */
struct int_arr {
  size_t size;
  int data[];    /* Defines array type. In our case `int`. */
} int_arr;

typedef struct int_arr* Int_TA;

/**
 * @brief    Create `Int_TA` with a give size.
 */
Int_TA
Int_array_new(size_t size)
{
  /*
   * `sizeof` ignores array size in case of incomplete array type, we must
   * explicitly include an appropriate size for the flexible array member when
   * allocating storage.
   */
  Int_TA new_arr = malloc(sizeof(new_arr) + sizeof(int) * size);

  if (new_arr == NULL) {
    return NULL;
  }

  new_arr->size = size;
  return new_arr;
}

/* ______________________________________________________________________________ */

int
main(void)
{
  Int_TA all = Int_array_new(42);

  /* Just to illustrate usage. */
  for (size_t i = 0; i < all->size; ++i) {
    all->data[i] = rand() % 43;
    printf("%d\n", all->data[i]);
  }

  free(all);

  return EXIT_SUCCESS;
}
