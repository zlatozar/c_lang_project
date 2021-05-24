#include <stdio.h>
#include <stdlib.h>

typedef int object_t;

typedef int number_t;

typedef int key_t;

typedef struct tr_n_t {
  key_t            key;
  struct tr_n_t*  left;
  struct tr_n_t* right;
  number_t     summand;
  int           height;
} tree_node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256
#define NEGINFTY -1000

tree_node_t* currentblock = NULL;
int size_left;
tree_node_t* free_list = NULL;

tree_node_t*
get_node()
{
  tree_node_t* tmp;

  if ( free_list != NULL ) {
    tmp = free_list;
    free_list = free_list -> left;

  } else {

    if ( currentblock == NULL || size_left == 0) {
      currentblock =
        (tree_node_t*) malloc( BLOCKSIZE * sizeof(tree_node_t) );
      size_left = BLOCKSIZE;
    }

    tmp = currentblock++;
    size_left -= 1;
  }

  return ( tmp );
}

void
return_node(tree_node_t* node)
{
  node->left = free_list;
  free_list = node;
}

tree_node_t*
create_tree(void)
{
  tree_node_t* tree;
  tree = get_node();
  tree->left = NULL;
  tree->summand = 0;

  /* need key -infty, use tree as non-NULL object ptr */
  insert( tree, NEGINFTY, (object_t*) tree );

  return ( tree );
}

void
left_rotation(tree_node_t* n)
{
  tree_node_t* tmp_node;
  key_t         tmp_key;   /* push down summand from n */

  n->left->summand += n->summand;
  n->right->summand += n->summand;
  n->summand = 0;

  tmp_node = n->right; /* push down summand from n->right */
  tmp_node->left->summand += tmp_node->summand;
  tmp_node->right->summand += tmp_node->summand;
  tmp_node->summand = 0;
  tmp_node = n->left;  /* perform normal left rotation */
  tmp_key  = n->key;

  n->left  = n->right;
  n->key   = n->right->key;
  n->right = n->left->right;
  n->left->right = n->left->left;
  n->left->left  = tmp_node;
  n->left->key   = tmp_key;
}

void
right_rotation(tree_node_t* n)
{
  tree_node_t* tmp_node;
  key_t        tmp_key; /* push down summand from n */

  n->left->summand += n->summand;
  n->right->summand += n->summand;
  n->summand = 0;

  tmp_node = n->left; /* push down summand from n->left */
  tmp_node->left->summand += tmp_node->summand;
  tmp_node->right->summand += tmp_node->summand;
  tmp_node->summand = 0;
  tmp_node = n->right;/* perform normal right rotation */
  tmp_key  = n->key;

  n->right = n->left;
  n->key   = n->left->key;
  n->left  = n->right->left;
  n->right->left = n->right->right;
  n->right->right  = tmp_node;
  n->right->key   = tmp_key;
}

object_t*
find(tree_node_t* tree, key_t query_key)
{
  tree_node_t* tmp_node;

  if ( tree->left == NULL )
  { return (NULL); }

  else {
    tmp_node = tree;

    while ( tmp_node->right != NULL ) {

      if ( query_key < tmp_node->key )
      { tmp_node = tmp_node->left; }

      else
      { tmp_node = tmp_node->right; }
    }

    if ( tmp_node->key == query_key )
    { return ( (object_t*) tmp_node->left ); }

    else
    { return ( NULL ); }
  }
}

number_t
evaluate_sum(tree_node_t* tree, key_t query_key)
{
  tree_node_t* current_tree_node;
  number_t sum;

  if ( tree->left == NULL )
  { return (0); }

  else {
    current_tree_node = tree;
    sum = tree->summand;

    while ( current_tree_node->right != NULL ) {

      if ( query_key < current_tree_node->key )
      { current_tree_node = current_tree_node->left; }

      else
      { current_tree_node = current_tree_node->right; }
      sum += current_tree_node->summand;
    }

    return ( sum );
  }
}

void
insert_interval(tree_node_t* tree, key_t a, key_t b, number_t w)
{
  tree_node_t* tmp_node;

  if ( find(tree, a) == NULL ) {
    insert( tree, a, (object_t*) tree );
  }  /* used treenode itself as non-NULL object pointer*/

  if ( find(tree, b) == NULL ) {
    insert( tree, b, (object_t*) tree );
  }

  tmp_node = tree; /* follow search path for a,*/
  while ( tmp_node->right != NULL ) {

    /* add w to everything right of path */
    if ( a < tmp_node->key ) {
      tmp_node->right->summand += w;
      tmp_node = tmp_node->left;

    } else
    { tmp_node = tmp_node->right; }
  }

  tmp_node->summand += w; /* leaf with key a */
  tmp_node = tree; /* follow search path for b, */

  while ( tmp_node->right != NULL ) {

    /* subtract w from everything right of path */
    if ( b < tmp_node->key ) {
      tmp_node->right->summand -= w;
      tmp_node = tmp_node->left;

    } else
    { tmp_node = tmp_node->right; }
  }

  tmp_node->summand -= w; /* leaf with key b */
}

int
insert(tree_node_t* tree, key_t new_key, object_t* new_object)
{
  tree_node_t* tmp_node;
  int finished;

  if ( tree->left == NULL ) {
    tree->left = (tree_node_t*) new_object;
    tree->key  = new_key;
    tree->height  = 0;
    tree->summand = 0;
    tree->right   = NULL;

  } else {
    tree_node_t* path_stack[100];
    int  path_st_p = 0;
    tmp_node = tree;

    while ( tmp_node->right != NULL ) {
      path_stack[path_st_p++] = tmp_node;

      if ( new_key < tmp_node->key )
      { tmp_node = tmp_node->left; }

      else
      { tmp_node = tmp_node->right; }
    }

    /* found the candidate leaf. Test whether key distinct */
    if ( tmp_node->key == new_key )
    { return ( -1 ); }

    /* key is distinct, now perform the insert */
    {
      tree_node_t* old_leaf, *new_leaf;
      old_leaf = get_node();
      old_leaf->left = tmp_node->left;
      old_leaf->key  = tmp_node->key;
      old_leaf->right   = NULL;
      old_leaf->height  = 0;
      old_leaf->summand = 0;

      new_leaf = get_node();
      new_leaf->left = (tree_node_t*) new_object;
      new_leaf->key  = new_key;
      new_leaf->right   = NULL;
      new_leaf->height  = 0;
      new_leaf->summand = 0;

      if ( tmp_node->key < new_key ) {
        tmp_node->left  = old_leaf;
        tmp_node->right = new_leaf;
        tmp_node->key = new_key;

      } else {
        tmp_node->left  = new_leaf;
        tmp_node->right = old_leaf;
      }

      tmp_node->height = 1;
    }

    /* rebalance */
    finished = 0;
    while ( path_st_p > 0 && !finished ) {
      int tmp_height, old_height;
      tmp_node = path_stack[--path_st_p];
      old_height = tmp_node->height;

      if ( tmp_node->left->height - tmp_node->right->height == 2 ) {

        if ( tmp_node->left->left->height - tmp_node->right->height == 1 ) {
          right_rotation( tmp_node );
          tmp_node->right->height = tmp_node->right->left->height + 1;
          tmp_node->height = tmp_node->right->height + 1;

        } else {
          left_rotation( tmp_node->left );
          right_rotation( tmp_node );

          tmp_height = tmp_node->left->left->height;
          tmp_node->left->height  = tmp_height + 1;
          tmp_node->right->height = tmp_height + 1;
          tmp_node->height = tmp_height + 2;
        }

      } else if ( tmp_node->left->height - tmp_node->right->height == -2 ) {

        if ( tmp_node->right->right->height - tmp_node->left->height == 1 ) {
          left_rotation( tmp_node );
          tmp_node->left->height = tmp_node->left->right->height + 1;
          tmp_node->height = tmp_node->left->height + 1;

        } else {
          right_rotation( tmp_node->right );
          left_rotation( tmp_node );
          tmp_height = tmp_node->right->right->height;
          tmp_node->left->height  = tmp_height + 1;
          tmp_node->right->height = tmp_height + 1;
          tmp_node->height = tmp_height + 2;
        }

      } else { /* update height even if there was no rotation */

        if ( tmp_node->left->height > tmp_node->right->height )
        { tmp_node->height = tmp_node->left->height + 1; }

        else
        { tmp_node->height = tmp_node->right->height + 1; }
      }

      if ( tmp_node->height == old_height )
      { finished = 1; }
    }
  }

  return ( 0 );
}

object_t*
delete (tree_node_t* tree, key_t delete_key)
{
  tree_node_t* tmp_node, *upper_node, *other_node;
  object_t* deleted_object;
  int finished;

  if ( tree->left == NULL )
  { return ( NULL ); }

  else if ( tree->right == NULL ) {

    if ( tree->key == delete_key ) {
      deleted_object = (object_t*) tree->left;
      tree->left = NULL;
      return ( deleted_object );

    } else
    { return ( NULL ); }

  } else {

    tree_node_t* path_stack[100];
    int path_st_p = 0;
    tmp_node = tree;

    while ( tmp_node->right != NULL ) {
      path_stack[path_st_p++] = tmp_node;
      upper_node = tmp_node;

      if ( delete_key < tmp_node->key ) {
        tmp_node   = upper_node->left;
        other_node = upper_node->right;

      } else {
        tmp_node   = upper_node->right;
        other_node = upper_node->left;
      }
    }

    if ( tmp_node->key != delete_key )
    { deleted_object = NULL; }

    else {
      upper_node->key   = other_node->key;
      upper_node->left  = other_node->left;
      upper_node->right = other_node->right;
      upper_node->height = other_node->height;
      deleted_object = (object_t*) tmp_node->left;
      return_node( tmp_node );
      return_node( other_node );

    }

    /*start rebalance*/
    finished = 0;
    path_st_p -= 1;

    while ( path_st_p > 0 && !finished ) {
      int tmp_height, old_height;
      tmp_node = path_stack[--path_st_p];
      old_height = tmp_node->height;

      if ( tmp_node->left->height - tmp_node->right->height == 2 ) {

        if ( tmp_node->left->left->height - tmp_node->right->height == 1 ) {
          right_rotation( tmp_node );
          tmp_node->right->height = tmp_node->right->left->height + 1;
          tmp_node->height = tmp_node->right->height + 1;

        } else {
          left_rotation( tmp_node->left );
          right_rotation( tmp_node );
          tmp_height = tmp_node->left->left->height;
          tmp_node->left->height  = tmp_height + 1;
          tmp_node->right->height = tmp_height + 1;
          tmp_node->height = tmp_height + 2;
        }

      } else if ( tmp_node->left->height -
                  tmp_node->right->height == -2 ) {

        if ( tmp_node->right->right->height - tmp_node->left->height == 1 ) {
          left_rotation( tmp_node );
          tmp_node->left->height = tmp_node->left->right->height + 1;
          tmp_node->height = tmp_node->left->height + 1;

        } else {
          right_rotation( tmp_node->right );
          left_rotation( tmp_node );
          tmp_height = tmp_node->right->right->height;
          tmp_node->left->height  = tmp_height + 1;
          tmp_node->right->height = tmp_height + 1;
          tmp_node->height = tmp_height + 2;
        }

      } else { /* update height even if there was no rotation */

        if ( tmp_node->left->height > tmp_node->right->height )
        { tmp_node->height = tmp_node->left->height + 1; }

        else
        { tmp_node->height = tmp_node->right->height + 1; }
      }

      if ( tmp_node->height == old_height )
      { finished = 1; }
    }
    /*end rebalance*/
    return ( deleted_object );
  }
}

void
check_tree( tree_node_t* tr, int depth, int lower, int upper )
{
  if ( tr->left == NULL )
  { printf("Tree Empty\n"); return; }

  if ( tr->key < lower || tr->key >= upper )
  { printf("Wrong Key Order \n"); }

  if ( tr->right == NULL ) {
    printf("%d(%d)  ", tr->key, depth );

  } else {
    check_tree(tr->left, depth + 1, lower, tr->key );
    check_tree(tr->right, depth + 1, tr->key, upper );
  }
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  tree_node_t* ws_tree;
  char nextop;
  ws_tree = create_tree();
  printf("Made Tree: weighted sum of intervals tree\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int ins_a, ins_b, ins_w, success;
      scanf(" [%d,%d[ %d", &ins_a, &ins_b, &ins_w);
      insert_interval( ws_tree, ins_a, ins_b, ins_w );
      printf("  inserted [%d,%d[with weight %d. Height now %d\n",
             ins_a, ins_b, ins_w, ws_tree->height );
    }

    if ( nextop == 'e' ) {
      int eval_key, value;
      scanf(" %d", &eval_key);
      value = evaluate_sum( ws_tree, eval_key);
      printf(" the sum at key %d is %d\n", eval_key, value);
    }
    /*     if( nextop == 'd' )
    { int delkey, *delobj;
      scanf(" %d", &delkey);
      delobj = delete( searchtree, delkey);
      if( delobj == NULL )
        printf("  delete failed for key %d\n", delkey);
      else
        printf("  delete successful, deleted object %d, height is now %d\n",
            *delobj, searchtree->height);
      }*/

    if ( nextop == '?' ) {
      printf("  Checking tree\n");
      check_tree(ws_tree, 0, -10000, 10000);
      printf("\n");

      if ( ws_tree->left != NULL )
        printf("key in root is %d, height of tree is %d\n",
               ws_tree->key, ws_tree->height );
      printf("  Finished Checking tree\n");
    }
  }

  return (0);
}
