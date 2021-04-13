#include "data_structs/sparse_matrix.h"

#include "lang/memory.h"
#include "lang/assert.h"
#include "lang/except.h"
#include "logger/log.h"

struct matrix_node {
  int row, column;
  Generic_T datapointer;
  Matrix_T nextrow, prevrow;
  Matrix_T nextcol, prevcol;
};

#define ROW(p_matrix)     ((p_matrix)->row)
#define NEXTROW(p_matrix) ((p_matrix)->nextrow)
#define PREVROW(p_matrix) ((p_matrix)->prevrow)

#define DATA(p_matrix)    ((p_matrix)->datapointer)

#define COL(p_matrix)     ((p_matrix)->column)
#define NEXTCOL(p_matrix) ((p_matrix)->nextcol)
#define PREVCOL(p_matrix) ((p_matrix)->prevcol)

/* ______________________________________________________________________________ */
/*                                                                         Local  */

typedef enum { CREATE_HEADER, FIND_HEADER } oper_type_et;

static void
__allocate_matrix_node(Matrix_T* p_matrix, int row, int col, Generic_T data)
{
  Matrix_T matrix;
  NEW(matrix);

  *p_matrix = matrix;

  ROW(matrix) = row;
  COL(matrix) = col;
  DATA(matrix) = data;

  NEXTROW(matrix) = NULL;
  PREVROW(matrix) = NULL;
  NEXTCOL(matrix) = NULL;
  PREVCOL(matrix) = NULL;
}

static void
__free_matrix_node(mnode_t** pp_mnode)
{
  FREE(pp_mnode);
}

/* Find the existing column node <= col. */
static Matrix_T
__get_previous_column(Matrix_T* p_matrix_head, int col)
{
  mnode_t* p_node;

  for (p_node = *p_matrix_head; COL(p_node) <= col && NEXTCOL(p_node) != NULL; )
  { p_node = NEXTCOL(p_node); }

  return (COL(p_node) > col) ? PREVCOL(p_node) : p_node;
}

static Matrix_T
__get_previous_row(Matrix_T* p_matrix_head, int row)
{
  mnode_t* p_node;

  for (p_node = *p_matrix_head; ROW(p_node) <= row && NEXTROW(p_node) != NULL; )
  { p_node = NEXTROW(p_node); }

  return (ROW(p_node) > row) ? PREVROW(p_node) : p_node;
}

/*
 * Find a specific column header node. If it doesn't exist, optionally create it
 * depending on `header_oper`. If `header_oper` is FIND_HEADER, return
 * FALSE if it doesn't exist. If `header_oper` is CREATE_HEADER, create the
 * header, add it to the linked list of headers, and return that new node.
 */
static bool
__find_col_head(oper_type_et header_oper, Matrix_T* p_matrix, int col, mnode_t** pp_headernode__)
{
  Matrix_T header;
  header = __get_previous_column(p_matrix, col);

  if (COL(header) == col) {
    *pp_headernode__ = header;
    return true;

  } else if (header_oper == FIND_HEADER) {
    *pp_headernode__ = NULL;
    return false;

  } else { /* Create header. */
    Matrix_T newhead;
    __allocate_matrix_node(&newhead, -1, col, NULL);

    NEXTCOL(newhead) = NEXTCOL(header);

    if (NEXTCOL(header) != NULL) {
      PREVCOL(NEXTCOL(header)) = newhead;
    }

    PREVCOL(newhead) = header;
    NEXTCOL(header) = newhead;
    *pp_headernode__ = newhead;

    return true;
  }

  /* Just in case. */
  return false;
}

/*
 * Find a specific row header node. If it doesn't exist, optionally create it
 * depending on `header_oper`. If `header_oper` is FIND_HEADER, return ERROR if
 * it doesn't exist. If `header_oper` is CREATE_HEADER, create the header, add
 * it to the linked list of headers, and return that new node.
 */
static bool
__find_row_head(oper_type_et header_oper, Matrix_T* p_matrix, int row, mnode_t** pp_headernode__)
{
  Matrix_T header;
  header = __get_previous_row(p_matrix, row);

  if (ROW(header) == row) {
    *pp_headernode__ = header;
    return true;

  } else if (header_oper == FIND_HEADER) {
    *pp_headernode__ = NULL;
    return ERROR;

  } else { /* Create header. */
    Matrix_T newhead;
    __allocate_matrix_node(&newhead, row, -1, NULL);

    NEXTROW(newhead) = NEXTROW(header);
    if (NEXTROW(header) != NULL)
    { PREVROW(NEXTROW(header)) = newhead; }

    PREVROW(newhead) = header;
    NEXTROW(header) = newhead;
    *pp_headernode__ = newhead;

    return true;
  }

  return false;
}

/* ______________________________________________________________________________ */

Matrix_T
Matrix_new()
{
  Matrix_T new_matrix;
  __allocate_matrix_node(&new_matrix, -1, -1, NULL);

  return new_matrix;
}

void
Matrix_put(Matrix_T* p_matrix, int row, int col, Generic_T value)
{
  Require(row >= 0);
  Require(col >= 0);

  Matrix_T colheader, rowheader;
  Matrix_T colprev, rowprev;
  Matrix_T newnode;

  __allocate_matrix_node(&newnode, row, col, value);

  // *INDENT-OFF*
  TRY
    __find_col_head(CREATE_HEADER, p_matrix, col, &colheader);
    __find_row_head(CREATE_HEADER, p_matrix, row, &rowheader);

  CATCH(Memory_Failed)
    Log_error("Memory allocation failed. Can't put matrix node.");
    __free_matrix_node(&newnode);
    RETHROW;

  END_TRY;
  // *INDENT-OFF*

  /*Insert in column list. */
  colprev = __get_previous_column(&rowheader, col);

  if (COL(colprev) == col) {
     DATA(colprev) = value;
     __free_matrix_node(&newnode);

     return;
  }

  NEXTCOL(newnode) = NEXTCOL(colprev);
  if (NEXTCOL(newnode) != NULL) {
    PREVCOL(NEXTCOL(newnode)) = newnode;
  }
  PREVCOL(newnode) = colprev;
  NEXTCOL(colprev) = newnode;

  /* Insert in row list. */
  rowprev = __get_previous_row(&colheader, row);

  NEXTROW(newnode) = NEXTROW(rowprev);
  if (NEXTROW(newnode) != NULL) {
    PREVROW(NEXTROW(newnode)) = newnode;
  }

  PREVROW(newnode) = rowprev;
  NEXTROW(rowprev) = newnode;
}

bool
Matrix_get(Matrix_T* p_matrix, int row, int col, Generic_T* p_value__)
{
  Require(row >= 0);
  Require(col >= 0);

  Matrix_T colheader;
  Matrix_T rowprev;

  if (!__find_col_head(FIND_HEADER, p_matrix, col, &colheader)) {
    return false;
  }

  rowprev = __get_previous_row(&colheader, row);

  if (ROW(rowprev) != row)
    return false;

  *p_value__ = DATA(rowprev);
  return true;
}

bool
Matrix_clear(Matrix_T* p_matrix, int row, int col)
{
  Require(row >= 0);
  Require(col >= 0);

  mnode_t* element;
  mnode_t* colheader;

  if (!__find_col_head(FIND_HEADER, p_matrix, col, &colheader)) {
    return false;
  }

  element = __get_previous_row(&colheader, row);
  if (ROW(element) != row)
    return false;

  NEXTROW(PREVROW(element)) = NEXTROW(element);
  if (NEXTROW(element) == NULL)
    PREVROW(NEXTROW(element)) = PREVROW(element);

  NEXTCOL(PREVCOL(element)) = NEXTCOL(element);
  if (NEXTCOL(element) != NULL)
    PREVCOL(NEXTCOL(element)) = PREVCOL(element);

  __free_matrix_node(&element);

  return true;
}
