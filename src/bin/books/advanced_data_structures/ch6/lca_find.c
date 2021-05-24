#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

typedef int key_t;

typedef struct lca_n_t {
  int            depth;
  item_t*         item;
  struct lca_n_t*   up;
  struct lca_n_t* next;
  struct lca_n_t* prev;
} lca_node_t;

typedef lca_node_t  object_t;

typedef struct tr_n_t {
  key_t            key;
  struct tr_n_t*  left;
  struct tr_n_t* right;
  int           height;
} tree_node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256

lca_node_t* currentblock = NULL;
int size_left;
lca_node_t* free_list = NULL;

tree_node_t*
get_node()
{
  lca_node_t* tmp;

  if ( free_list != NULL ) {
    tmp = free_list;
    free_list = free_list ->up;

  } else {

    if ( currentblock == NULL || size_left == 0) {
      currentblock =
        (lca_node_t*) malloc( BLOCKSIZE * sizeof(lca_node_t) );
      size_left = BLOCKSIZE;
    }

    tmp = currentblock++;
    size_left -= 1;
  }

  return ( (tree_node_t*) tmp );
}

void
return_node(tree_node_t* node)
{
  lca_node_t* tmp;
  tmp = (lca_node_t*) node;
  tmp->up = free_list;
  free_list = tmp;
}

lca_node_t*
get_lca_node()
{
  return ( (lca_node_t*) get_node() );
}

void
return_lca_node(lca_node_t* node)
{
  return_node( (tree_node_t*) node );
}
/*-----------------------------------------------------*/

lca_node_t*
create_tree(item_t* new_item)
{
  lca_node_t* new_node;
  new_node = get_lca_node();
  new_node->item = new_item;
  new_node->depth = 0;
  new_node->prev = NULL;
  new_node->up = NULL;
  new_node->next = NULL;

  return ( new_node );
}

lca_node_t*
add_leaf(lca_node_t* node, item_t* new_item)
{
  lca_node_t* new_node;
  /* create tree node */
  new_node = get_lca_node();
  new_node->item = new_item;
  new_node->depth = node->depth + 1;
  new_node->up = node;
  new_node->prev = NULL;

  /* now create new list of forward pointers */
  {
    lca_node_t* tmp;
    int i;
    tmp = new_node;

    for ( i = new_node->depth; i > 1 ; i /= 2 ) {
      /* add node to new_node list */
      tmp->next = get_lca_node();
      tmp->next->prev = tmp;
      tmp->next->depth = tmp->depth;

      if ( tmp->up->up->next == NULL ) {
        /* create new target node */
        tmp->up->up->next = get_lca_node();
        tmp->up->up->next->prev = tmp->up->up;
        tmp->up->up->next->depth = tmp->up->up->depth;
        tmp->up->up->next->next = NULL;
        tmp->up->up->next->up = NULL;
      } /* now set forward pointer */

      tmp->next->up = tmp->up->up->next;
      tmp = tmp->next;
    } /* and finish list */

    tmp->next = NULL;
  }
  return ( new_node );
}

int
depth(lca_node_t* node)
{
  return ( node->depth );
}

lca_node_t*
lca(lca_node_t* node1, lca_node_t* node2)
{
  lca_node_t* tmp;
  int diff;

  if ( node1->depth < node2->depth ) {
    tmp = node1;
    node1 = node2;
    node2 = tmp;
  } /* now node1 has larger depth. Move up to the same depth */

  {
    int diff;
    diff = node1->depth - node2->depth;

    while ( diff > 1 ) {

      if ( diff % 2 == 1 )
      { node1 = node1->up->next; }

      else
      { node1 = node1->next; }

      diff /= 2;
    }

    if ( diff == 1 )
    { node1 = node1->up; }

    while ( node1->prev != NULL )
    { node1 = node1->prev; } /* move back to beginning of list */

  } /* now both nodes at same depth */

  if ( node1 == node2 )
  { return ( node1 ); }

  /* if not the same, perform exponential search */
  {
    int current_depth, step_size;
    current_depth = node1->depth;
    step_size = 1;

    while ( current_depth >= 2 * step_size ) {
      node1 = node1->next;
      node2 = node2->next;
      step_size *= 2;
    }  /* maximum stepsize, now go up, and decrease stepsize */

    while ( current_depth >= 1 ) {

      if ( step_size > current_depth ) {
        node1 = node1->prev;
        node2 = node2->prev;
        step_size /= 2; /* steps too large, halve size */

      } else if ( node1->up != node2->up ) {
        node1 = node1->up; /* step up still below lca */
        node2 = node2->up;
        current_depth -= step_size;

      } else { /* node1->up == node2->up */

        if ( step_size > 1) { /* upper bound for lca */
          node1 = node1->prev;
          node2 = node2->prev;
          step_size /= 2;
        } else /* immediately below lca */

        { return ( node1->up ); }
      }
    }
    return ( NULL ); /* different trees */
  }
}

/*-----------------------------------------------------*/
tree_node_t*
create_s_tree(void)
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

int
insert(tree_node_t* tree, key_t new_key, object_t* new_object)
{
  tree_node_t* tmp_node;
  int finished;

  if ( tree->left == NULL ) {
    tree->left = (tree_node_t*) new_object;
    tree->key  = new_key;
    tree->height = 0;
    tree->right  = NULL;

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
      old_leaf->key = tmp_node->key;
      old_leaf->right  = NULL;
      old_leaf->height = 0;
      new_leaf = get_node();
      new_leaf->left = (tree_node_t*) new_object;
      new_leaf->key = new_key;
      new_leaf->right  = NULL;
      new_leaf->height = 0;

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
    /*end rebalance*/
    return ( deleted_object );
  }
}

void
check_tree(tree_node_t* tr, int depth, int lower, int upper)
{
  if ( tr->left == NULL )
  { printf("Tree Empty\n"); return; }

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
  searchtree = create_s_tree();
  printf("Test lca structure\n");
  {
    int* new_item;
    lca_node_t* lca_n;
    new_item = (int*) malloc(sizeof(int));
    *new_item = 0;
    lca_n = create_tree( new_item);
    insert( searchtree, 0, lca_n );
  }

  printf("Created root; root key 0\n");
  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'a' ) {
      int* new_item, old_item;
      lca_node_t* new_node, *old_node;
      int success;
      new_item = (int*) malloc(sizeof(int));
      scanf(" %d %d", new_item, &old_item);
      old_node = find(searchtree, old_item);

      if ( old_node != NULL ) {
        printf("  found node with key %d\n", old_item);
        new_node = add_leaf( old_node, new_item );
        printf("  added new leaf with key %d below tha node\n", *new_item);
        success = insert( searchtree, *new_item, new_node );

        if ( success == 0 )
        { printf("  inserted item %d successfully\n", *new_item); }

        else
        { printf("  insert failed, success = %d\n", success); }
      }
    }

    if ( nextop == 'l' ) {
      int item1, item2;
      lca_node_t* n1, *n2, *result;
      scanf(" %d %d", &item1, &item2);
      n1 = find( searchtree, item1);
      n2 = find( searchtree, item2);

      if ( n1 != NULL && n2 != NULL ) {
        printf(" found both nodes.");
        result = lca( n1, n2 );

        if ( result != NULL )
        { printf(" the lca has item %d\n", *(result->item) ); }

        else
        { printf(" received invalid node\n"); }

      } else
      { printf(" did not find the requested nodes \n"); }

    }

    if ( nextop == 'd' ) {
      int item;
      lca_node_t* n;
      scanf(" %d", &item);
      n = find( searchtree, item);
      printf("  the depth of item %d is %d\n", item, depth(n) );
    }

    if ( nextop == '?' ) {
      int item;
      lca_node_t* n, *m;
      scanf(" %d", &item);
      printf("looking for node with key %d\n", item);
      n = find( searchtree, item);

      if (n == NULL )
      { printf(" wrong node key\n"); }

      else {
        printf(" found node %d. The noes above it are\n", item );

        while ( n->up != NULL ) {
          n = n->up;
          printf(" %d", *(n->item));
        }

        printf("\n");
      }
    }
  }/* end of while-loop*/

  return (0);
}
