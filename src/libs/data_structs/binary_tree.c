#include "data_structs/binary_tree.h"

#include "lang/memory.h"

struct tree_node {
  Generic_T datapointer;
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
__allocate_tree_node(btnode_t** pp_btnode, Generic_T data)
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
