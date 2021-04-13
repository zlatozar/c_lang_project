/**
 * @file    binary_tree.h
 * @brief   Binary tree ADT interface.
 */
#if !defined(DATA_STRUCTS_BINARY_TREE_H)
#define DATA_STRUCTS_BINARY_TREE_H

#include "lang/extend.h"

typedef struct tree_node btnode_t;
typedef btnode_t* BinTree_T;

typedef enum { PRE_ORDER, IN_ORDER, P0ST_0RDER } order_et;

/**
 * Initialize an empty tree.
 */
extern BinTree_T BinTree_new(void);

/**
 * Check if given tree is empty.
 */
extern bool BinTree_is_empty(BinTree_T btree);

#endif  /* DATA_STRUCTS_BINARY_TREE_H */
