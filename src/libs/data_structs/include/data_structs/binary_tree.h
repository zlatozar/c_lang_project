/**
 * @file    binary_tree.h
 * @brief   Binary tree ADT interface.
 */
#if !defined(DATA_STRUCTS_BINARY_TREE_H)
#define DATA_STRUCTS_BINARY_TREE_H

#include "lang/extend.h"

typedef struct tree_node btnode_t;
typedef btnode_t* BinTree_T;

typedef enum { PRE_ORDER, IN_ORDER, POST_ORDER } order_et;

/**
 * Initialize an empty tree.
 */
extern BinTree_T BinTree_new(void);

/**
 * Check if given tree is empty.
 */
extern bool BinTree_is_empty(BinTree_T btree);

/**
 * Allocate a new node and make that the root of a tree.
 */
extern void BinTree_make_root(BinTree_T* p_btree, Generic_T data, BinTree_T left, BinTree_T right);

/**
 * Traverse a tree in preorder, postorder, or inorder using DFS.
 */
extern bool BinTree_traverse(BinTree_T btree, bool (*apply_fn)(Generic_T), order_et order);

/*
 * Delete an entire tree (uses a postorder traversal), calling
 * `free_data_fn` with the data stored at each node.
 */
extern void BinTree_destroy(BinTree_T* p_btree, free_data_FN free_data_fn);

extern void BinTree_free(BinTree_T* p_btree);

#endif  /* DATA_STRUCTS_BINARY_TREE_H */
