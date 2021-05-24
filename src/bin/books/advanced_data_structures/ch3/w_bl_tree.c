#include <stdio.h>
#include <stdlib.h>

typedef int object_t;

typedef int key_t;

typedef struct tr_n_t {
  key_t        key;
  struct tr_n_t*  left;
  struct tr_n_t* right;
  int           weight;
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

#define ALPHA 0.288
#define EPSILON 0.005

int
insert(tree_node_t* tree, key_t new_key, object_t* new_object)
{
  tree_node_t* tmp_node;

  if ( tree->left == NULL ) {
    tree->left = (tree_node_t*) new_object;
    tree->key  = new_key;
    tree->weight = 1;
    tree->right  = NULL;

  } else {
    tree_node_t* path_stack[100];
    int  path_st_p = 0;
    tmp_node = tree;

    while ( tmp_node->right != NULL ) {
      path_stack[path_st_p++] = tmp_node;

      if (  new_key < tmp_node->key )
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
      old_leaf->key = tmp_node->key;
      old_leaf->right  = NULL;
      old_leaf->weight = 1;

      new_leaf = get_node();
      new_leaf->left = (tree_node_t*) new_object;
      new_leaf->key = new_key;
      new_leaf->right  = NULL;
      new_leaf->weight = 1;

      if ( tmp_node->key < new_key ) {
        tmp_node->left  = old_leaf;
        tmp_node->right = new_leaf;
        tmp_node->key = new_key;

      } else {
        tmp_node->left  = new_leaf;
        tmp_node->right = old_leaf;
      }
      tmp_node->weight = 2;
    }

    /* rebalance */
    while ( path_st_p > 0 ) {
      tmp_node = path_stack[--path_st_p];
      tmp_node->weight = tmp_node->left->weight + tmp_node->right->weight;

      if ( tmp_node->right->weight < ALPHA * tmp_node->weight ) {

        if ( tmp_node->left->left->weight > (ALPHA + EPSILON)*tmp_node->weight ) {

          right_rotation( tmp_node );
          tmp_node->right->weight =
            tmp_node->right->left->weight
            + tmp_node->right->right->weight;

        } else {

          left_rotation( tmp_node->left );
          right_rotation( tmp_node );

          tmp_node->right->weight =
            tmp_node->right->left->weight
            + tmp_node->right->right->weight;

          tmp_node->left->weight =
            tmp_node->left->left->weight
            + tmp_node->left->right->weight;
        }

      } else if ( tmp_node->left->weight
                  < ALPHA * tmp_node->weight ) {

        if ( tmp_node->right->right->weight
             > (ALPHA + EPSILON)*tmp_node->weight ) {

          left_rotation( tmp_node );
          tmp_node->left->weight =
            tmp_node->left->left->weight
            + tmp_node->left->right->weight;

        } else {
          right_rotation( tmp_node->right );
          left_rotation( tmp_node );

          tmp_node->right->weight =
            tmp_node->right->left->weight
            + tmp_node->right->right->weight;

          tmp_node->left->weight =
            tmp_node->left->left->weight
            + tmp_node->left->right->weight;
        }
      }

    }
  }
  return ( 0 );
}

object_t*
delete (tree_node_t* tree, key_t delete_key)
{
  tree_node_t* tmp_node, *upper_node, *other_node;
  object_t* deleted_object;

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
      upper_node->weight = other_node->weight;

      deleted_object = (object_t*) tmp_node->left;

      return_node( tmp_node );
      return_node( other_node );
    }

    /*start rebalance*/
    path_st_p -= 1;
    while ( path_st_p > 0 ) {

      tmp_node = path_stack[--path_st_p];
      tmp_node->weight =  tmp_node->left->weight
                          + tmp_node->right->weight;

      if ( tmp_node->right->weight
           < ALPHA * tmp_node->weight ) {

        if ( tmp_node->left->left->weight
             > (ALPHA + EPSILON)*tmp_node->weight ) {

          right_rotation( tmp_node );
          tmp_node->right->weight =
            tmp_node->right->left->weight
            + tmp_node->right->right->weight;

        } else {

          left_rotation( tmp_node->left );
          right_rotation( tmp_node );
          tmp_node->right->weight =
            tmp_node->right->left->weight
            + tmp_node->right->right->weight;
          tmp_node->left->weight =
            tmp_node->left->left->weight
            + tmp_node->left->right->weight;
        }

      } else if ( tmp_node->left->weight
                  < ALPHA * tmp_node->weight ) {

        if ( tmp_node->right->right->weight
             > (ALPHA + EPSILON)*tmp_node->weight ) {
          left_rotation( tmp_node );
          tmp_node->left->weight =
            tmp_node->left->left->weight
            + tmp_node->left->right->weight;

        } else {
          right_rotation( tmp_node->right );
          left_rotation( tmp_node );
          tmp_node->right->weight =
            tmp_node->right->left->weight
            + tmp_node->right->right->weight;
          tmp_node->left->weight =
            tmp_node->left->left->weight
            + tmp_node->left->right->weight;
        }
      }
    }

    /*end rebalance*/
    return ( deleted_object );
  }
}

void
check_tree(tree_node_t* tr, int depth, int lower, int upper)
{

  if ( tr->left == NULL )
  {  printf("Tree Empty\n"); return; }

  if ( tr->key < lower || tr->key >= upper )
  { printf("Wrong Key Order \n"); }

  if ( tr->right == NULL ) {

    if ( *( (int*) tr->left) == 10 * tr->key + 2 )
    { printf("%d(%d)  ", tr->key, depth ); }

    else
    { printf("Wrong Object \n"); }

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
  tree_node_t* searchtree;
  char nextop;
  searchtree = create_tree();

  printf("Made Tree: Weight-Balanced Tree\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int inskey,  *insobj, success;
      insobj = (int*) malloc(sizeof(int));
      scanf(" %d", &inskey);
      *insobj = 10 * inskey + 2;
      success = insert( searchtree, inskey, insobj );

      if ( success == 0 )
        printf("  insert successful, key = %d, object value = %d, \
                  weight is %d\n",
               inskey, *insobj, searchtree->weight );
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
        printf("  delete successful, deleted object %d, weight is now %d\n",
               *delobj, searchtree->weight);
    }

    if ( nextop == '?' ) {
      printf("  Checking tree\n");
      check_tree(searchtree, 0, -1000, 1000);
      printf("\n");

      if ( searchtree->left != NULL )
        printf("key in root is %d, weight of tree is %d\n",
               searchtree->key, searchtree->weight );
      printf("  Finished Checking tree\n");
    }
  }
  return (0);
}
