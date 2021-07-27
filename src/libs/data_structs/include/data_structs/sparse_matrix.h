/**
 * @file    sparse_matrix.h
 * @brief   Sparse matrix ADT interface.
 */
#if !defined(DATA_STRUCTS_SPARSE_MATRIX_H)
#define DATA_STRUCTS_SPARSE_MATRIX_H

#include "lang/extend.h"

typedef struct matrix_node mnode_t;

typedef mnode_t* Matrix_T;

/**
 * Create a matrix by allocating the matrix header node [-1, -1].
 */
extern Matrix_T Matrix_new(void);

/**
 * p_matrix[row][col] = value;
 *   This is done by locating correct position in the column list
 *   and the row list and inserting the new node (containing value)
 *   in the correct position in each of those lists.
 */
extern void Matrix_put(Matrix_T* p_matrix, int row, int col, Object_T value);

/**
 * Set `p_value__` to the value at p_matrix[row][col].
 */
extern bool Matrix_get(Matrix_T* p_matirx, int row, int col, Object_T* p_value__);

/**
 * Free the space used by p_matrix[row][col]. This assumes the
 * application has already freed the space used by the DATA field!
 */
extern bool Matrix_clear(Matrix_T* p_matrix, int row, int col);

#endif  /* DATA_STRUCTS_SPARSE_MATRIX_H */
