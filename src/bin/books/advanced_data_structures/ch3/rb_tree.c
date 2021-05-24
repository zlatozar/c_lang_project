#include <stdio.h>
#include <stdlib.h>

typedef int object_t;

typedef int key_t;

typedef struct tr_n_t {
  key_t                key;
  struct tr_n_t*      left;
  struct tr_n_t*     right;
  enum {red, black}  color;
  /* possibly other information */
} tree_node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256

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
      currentblock = (tree_node_t*) malloc( BLOCKSIZE * sizeof(tree_node_t) );
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
  tree_node_t* tmp_node;

  tmp_node = get_node();
  tmp_node->left = NULL;
  tmp_node->color = black;

  return ( tmp_node );
}

void
left_rotation(tree_node_t* n)
{
  tree_node_t* tmp_node;
  key_t        tmp_key;

  tmp_node = n->left;
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
  key_t        tmp_key;

  tmp_node = n->right;
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

      if ( tmp_node->color == red )
      { printf("r"); }
      else
      { printf("b"); }

      if ( query_key < tmp_node->key )
      { tmp_node = tmp_node->left; }

      else
      { tmp_node = tmp_node->right; }
    }

    if ( tmp_node->color == red )
    { printf("r\n"); }

    else
    { printf("b\n"); }

    if ( tmp_node->key == query_key )
    { return ( (object_t*) tmp_node->left ); }

    else
    { return ( NULL ); }
  }
}

int
insert(tree_node_t* tree, key_t new_key, object_t* new_object)
{
  tree_node_t* current_node;
  int finished = 0;

  if ( tree->left == NULL ) {
    tree->left = (tree_node_t*) new_object;
    tree->key  = new_key;
    tree->color = black; /* root is always black */
    tree->right  = NULL;

  } else {
    tree_node_t* path_stack[100];
    int path_st_p = 0;
    current_node = tree;

    while ( current_node->right != NULL ) {
      path_stack[ path_st_p++ ] = current_node ;

      if ( new_key < current_node->key )
      { current_node = current_node->left; }

      else
      { current_node = current_node->right; }
    }

    /* found the candidate leaf. Test whether key distinct */
    if ( current_node->key == new_key )
    { return ( -1 ); }

    /* key is distinct, now perform the insert */
    {
      tree_node_t* old_leaf, *new_leaf;

      old_leaf = get_node();
      old_leaf->left = current_node->left;
      old_leaf->key = current_node->key;
      old_leaf->right  = NULL;
      old_leaf->color = red;

      new_leaf = get_node();
      new_leaf->left = (tree_node_t*) new_object;
      new_leaf->key = new_key;
      new_leaf->right  = NULL;
      new_leaf->color = red;

      if ( current_node->key < new_key ) {
        current_node->left  = old_leaf;
        current_node->right = new_leaf;
        current_node->key = new_key;

      } else {
        current_node->left  = new_leaf;
        current_node->right = old_leaf;
      }
    }

    /* rebalance */
    if ( current_node->color == black || current_node == tree )
    { finished = 1; }

    /* else: current_node is upper node of red-red conflict*/
    while ( path_st_p > 0 && !finished ) {
      tree_node_t* upper_node, *other_node;
      upper_node = path_stack[ --path_st_p ];

      if (upper_node->left->color == upper_node->right->color) {
        /* both red, and upper_node black */
        upper_node->left->color = black;
        upper_node->right->color = black;
        upper_node->color = red;

      } else { /* current_node red, other_node black */

        if ( current_node == upper_node->left) {
          other_node = upper_node->right;

          /* other_node->color == black */
          if ( current_node->right->color == black ) {
            right_rotation( upper_node );
            upper_node->right->color = red;
            upper_node->color = black;
            finished = 1;

          } else { /* current_node->right->color == red */
            left_rotation( current_node );
            right_rotation( upper_node );
            upper_node->right->color = black;
            upper_node->left->color = black;
            upper_node->color = red;
          }

        } else { /* current_node == upper_node->right */

          other_node = upper_node->left;

          /* other_node->color == black */
          if ( current_node->left->color == black ) {
            left_rotation( upper_node );
            upper_node->left->color = red;
            upper_node->color = black;
            finished = 1;

          } else { /* current_node->left->color == red */
            right_rotation( current_node );
            left_rotation( upper_node );
            upper_node->right->color = black;
            upper_node->left->color = black;
            upper_node->color = red;
          }
        } /* end current_node left/right of upper */

        current_node = upper_node;
      }  /*end other_node red/black */

      if ( !finished && path_st_p > 0 )
        /* upper is red, conflict possibly propagates upward */
      {
        current_node = path_stack[ --path_st_p ];

        if ( current_node->color == black )
        { finished = 1; } /* no conflict above */
        /* else current upper node of red-red conflict */
      }
    }  /* end while loop moving back to root */

    tree->color = black; /* root is always black */
  }

  return ( 0 );
}

object_t*
delete (tree_node_t* tree, key_t delete_key)
{
  tree_node_t* tmp_node, *upper, *other;
  object_t* deleted_object;
  int finished = 0;

  if ( tree->left == NULL )
  { return ( NULL ); }

  else if ( tree->right == NULL ) {

    if (  tree->key == delete_key ) {
      deleted_object = (object_t*) tree->left;
      tree->left = NULL;

      return ( deleted_object );

    } else {
      return ( NULL );
    }

  } else {

    tree_node_t* path_stack[100];
    int path_st_p = 0;
    tmp_node = tree;

    while ( tmp_node->right != NULL ) {
      path_stack[path_st_p++] = tmp_node;
      upper = tmp_node;

      if ( delete_key < tmp_node->key ) {
        tmp_node = upper->left;
        other    = upper->right;

      } else {
        tmp_node = upper->right;
        other    = upper->left;
      }
    }

    if ( tmp_node->key != delete_key ) {
      deleted_object = NULL;
      finished = 1;

    } else {
      upper->key   = other->key;
      upper->left  = other->left;
      upper->right = other->right;

      if (( upper->color == black && other->color == red ) ||
          ( upper->color == red && other->color == black ))
      { finished = 1; }

      /* else: both black, so we have a missing black */
      upper->color = black;
      deleted_object = (object_t*) tmp_node->left;

      return_node( tmp_node );
      return_node( other );
    }

    /*start rebalance*/
    while ( path_st_p > 0 && !finished ) {
      tmp_node = path_stack[--path_st_p];

      /* tmp_node is black-deficient */
      if ( tmp_node->color == red || tmp_node == tree) {
        tmp_node->color = black;
        finished = 1;

      } else { /* black node, black-deficient, and not root */
        upper = path_stack[--path_st_p];
        if ( tmp_node == upper->left ) {
          other = upper->right;

          if (  upper->color == black ) {

            if ( other->color == black ) {

              if ( other->left->color == black ) {
                left_rotation( upper );
                upper->left->color = red;
                upper->color = black;

              } else {
                right_rotation( other );
                left_rotation( upper );
                upper->left->color = black;
                upper->right->color = black;
                upper->color = black;
                finished = 1;
              }

            } else { /* other->color == red */

              if ( other->left->left->color == black ) {
                left_rotation( upper );
                left_rotation( upper->left );
                upper->left->left->color = red;
                upper->left->color = black;
                upper->color = black;
                finished = 1;

              } else {
                left_rotation( upper );
                right_rotation( upper->left->right );
                left_rotation( upper->left );
                upper->left->left->color = black;
                upper->left->right->color = black;
                upper->left->color = red;
                upper->color = black;
                finished = 1;
              }
            }

          } else { /* upper->color == red */

            if (  other->left->color == black ) {
              left_rotation( upper );
              upper->left->color = red;
              upper->color = black;
              finished = 1;

            } else {
              right_rotation( other );
              left_rotation( upper );
              upper->left->color = black;
              upper->right->color = black;
              upper->color = red;
              finished = 1;
            }
          }

        } else { /* tmp_node = upper->right */
          other = upper->left;

          if (  upper->color == black ) {

            if ( other->color == black ) {

              if ( other->right->color == black ) {
                right_rotation( upper );
                upper->right->color = red;
                upper->color = black;

              } else {
                left_rotation( other );
                right_rotation( upper );
                upper->left->color = black;
                upper->right->color = black;
                upper->color = black;
                finished = 1;
              }

            } else { /* other->color == red */

              if ( other->right->right->color == black ) {
                right_rotation( upper );
                right_rotation( upper->right );
                upper->right->right->color = red;
                upper->right->color = black;
                upper->color = black;
                finished = 1;

              } else {
                right_rotation( upper );
                left_rotation( upper->right->left );
                right_rotation( upper->right );
                upper->right->right->color = black;
                upper->right->left->color = black;
                upper->right->color = red;
                upper->color = black;
                finished = 1;
              }
            }

          } else { /* upper->color == red */

            if (  other->right->color == black ) {
              right_rotation( upper );
              upper->right->color = red;
              upper->color = black;
              finished = 1;

            } else {
              left_rotation( other );
              right_rotation( upper );
              upper->left->color = black;
              upper->right->color = black;
              upper->color = red;
              finished = 1;
            }
          }
        }
      }
    }

    /*end rebalance*/
    return ( deleted_object );
  }
}

void
check_tree(tree_node_t* tr, int count, int lower, int upper)
{
  if ( tr->left == NULL )
  {  printf("Tree Empty\n"); return; }

  if ( tr->key < lower || tr->key >= upper )
  { printf("Wrong Key Order \n"); }

  if ( tr->right == NULL ) { /* leaf */

    if ( *( (int*) tr->left) == 10 * tr->key + 2 )
    { printf("%d(%d)  ", tr->key, count + (tr->color == black) ); }

    else
    { printf("Wrong Object \n"); }

  } else { /* not leaf */

    if ( tr->color == red ) {

      if (  tr->left->color == red )
      { printf("wrong color below red key %d on the left\n", tr->key ); }

      if (  tr->right->color == red )
      { printf("wrong color below red key %d on the right\n", tr->key ); }
    }

    check_tree(tr->left, count + (tr->color == black), lower, tr->key );
    check_tree(tr->right, count + (tr->color == black), tr->key, upper );
  }
}

// _____________________________________________________________________________
//                                                                 Simple test

int
main()
{
  tree_node_t* searchtree;
  char nextop;
  searchtree = create_tree();

  printf("Made Tree: Red-Black Tree\n");

  while ( (nextop = getchar()) != 'q' ) {
    if ( nextop == 'i' ) {
      int inskey,  *insobj, success;
      insobj = (int*) malloc(sizeof(int));
      scanf(" %d", &inskey);
      *insobj = 10 * inskey + 2;

      success = insert( searchtree, inskey, insobj );

      if ( success == 0 )
        printf("  insert successful, key = %d, object value = %d,\n",
               inskey, *insobj );
      else
      { printf("  insert failed, success = %d\n", success); }
    }

    if ( nextop == 'f' ) {
      int findkey, *findobj;
      scanf(" %d", &findkey);
      findobj = find( searchtree, findkey);

      if ( findobj == NULL )
      { printf("  find failed, for key %d\n", findkey); }

      else
      { printf("  find successful, found object %d\n", *findobj); }
    }

    if ( nextop == 'd' ) {
      int delkey, *delobj;
      scanf(" %d", &delkey);
      delobj = delete ( searchtree, delkey);

      if ( delobj == NULL )
      { printf("  delete failed for key %d\n", delkey); }

      else
        printf("  delete successful, deleted object %d\n",
               *delobj);
    }

    if ( nextop == '?' ) {
      printf("  Checking tree\n");
      check_tree(searchtree, 0, -1000, 1000);
      printf("\n");

      if ( searchtree->left != NULL )
      { printf("key in root is %d,\n", searchtree->key); }

      printf("  Finished Checking tree\n");
    }
  }

  return (0);
}
