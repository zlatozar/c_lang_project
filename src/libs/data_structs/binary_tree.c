#include "data_structs/binary_tree.h"

#include "lang/memory.h"
#include "lang/assert.h"

struct tree_node {
  Object_T datapointer;
  BinTree_T left;
  BinTree_T right;
};

#define DATA(p_btnode)  ((p_btnode)->datapointer)
#define LEFT(p_btnode)  ((p_btnode)->left)
#define RIGHT(p_btnode) ((p_btnode)->right)

/* ______________________________________________________________________________ */
/*                                                                         Local  */

/*
 * Allocate a tree node and initialize the DATA field with data.
 */
static void
__allocate_tree_node(btnode_t** pp_btnode, Object_T data)
{
  btnode_t* newnode;
  NEW(newnode);

  *pp_btnode = newnode;

  DATA(newnode) = data;
  LEFT(newnode) = NULL;
  RIGHT(newnode) = NULL;
}

static void
__free_tree_node(btnode_t** pp_btnode)
{
  FREE(*pp_btnode);
}

/* __________________________________________________________________________ */

BinTree_T
BinTree_new(void)
{
  BinTree_T btree = NULL;
  return btree;
}

bool
BinTree_is_empty(BinTree_T btree)
{
  return btree == NULL;
}

/* Build like a LEGO. */
void
BinTree_make_root(BinTree_T* p_btree, Object_T data, BinTree_T left, BinTree_T right)
{
  if (!BinTree_is_empty(*p_btree)) {
    Require(0 && "Passed tree is not empty");
  }

  __allocate_tree_node(p_btree, data);

  LEFT(*p_btree) = left;
  RIGHT(*p_btree) = right;
}

static bool
__preorder_traverse(BinTree_T btree, bool (*apply_fn)(Object_T))
{
  Require(apply_fn);

  bool result;

  if (BinTree_is_empty(btree))
  { return true; }

  /* First process data. */
  result = apply_fn(DATA(btree));

  if (result) {
    result = __preorder_traverse(LEFT(btree), apply_fn);
  }

  if (result) {
    result = __preorder_traverse(RIGHT(btree), apply_fn);
  }

  return result;
}

static bool
__inorder_traverse(BinTree_T btree, bool (*apply_fn)(Object_T))
{
  Require(apply_fn);

  bool result;

  if (BinTree_is_empty(btree))
  { return true; }

  result = __inorder_traverse(LEFT(btree), apply_fn);

  /* Process data between. */
  if (result) {
    result = apply_fn(DATA(btree));
  }

  if (result) {
    result = __inorder_traverse(RIGHT(btree), apply_fn);
  }

  return result;
}

static bool
__postorder_traverse(BinTree_T btree, bool (*apply_fn)(Object_T))
{
  Require(apply_fn);

  bool result;

  if (BinTree_is_empty(btree))
  { return true; }

  result = __postorder_traverse(LEFT(btree), apply_fn);

  if (result) {
    result = __postorder_traverse(RIGHT(btree), apply_fn);
  }

  /* Process data at last. */
  if (result) {
    result = apply_fn(DATA(btree));
  }

  return result;
}

bool
BinTree_traverse(BinTree_T btree, bool (*apply_fn)(Object_T), order_et order)
{
  switch (order) {
    case PRE_ORDER:
      return __preorder_traverse(btree, apply_fn);

    case IN_ORDER:
      return __inorder_traverse(btree, apply_fn);

    case POST_ORDER:
      return __postorder_traverse(btree, apply_fn);

    default:
      Require(0 && "Unknown traverse order.");
  }

  return false;
}

/* Not tail recursive. */
void
BinTree_destroy(BinTree_T* p_btree, free_data_FN free_data_fn)
{
  if (!BinTree_is_empty(*p_btree)) {
    BinTree_destroy(&LEFT(*p_btree), free_data_fn);
    BinTree_destroy(&RIGHT(*p_btree), free_data_fn);

    if (free_data_fn != NULL) {
      free_data_fn(DATA(*p_btree));
    }

    __free_tree_node(p_btree);
  }
}

void
BinTree_free(BinTree_T* p_btree)
{
  BinTree_destroy(p_btree, NULL);
}
