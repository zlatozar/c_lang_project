#include <stdio.h>
#include <stdlib.h>

typedef int item_t;

typedef int key_t;

typedef struct uf_n_t {
  int          height;
  int        indegree;
  item_t*        item;
  struct uf_n_t*   up;
  struct uf_n_t* list;
} uf_node_t;

typedef uf_node_t  object_t;

typedef struct tr_n_t {
  key_t            key;
  struct tr_n_t*  left;
  struct tr_n_t* right;
  int           height;
} tree_node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256

uf_node_t* currentblock = NULL;
int size_left;
uf_node_t* free_list = NULL;

tree_node_t*
get_node()
{
  uf_node_t* tmp;

  if ( free_list != NULL ) {
    tmp = free_list;
    free_list = free_list ->up;

  } else {

    if ( currentblock == NULL || size_left == 0) {
      currentblock =
        (uf_node_t*) malloc( BLOCKSIZE * sizeof(uf_node_t) );
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
  uf_node_t* tmp;
  tmp = (uf_node_t*) node;
  tmp->up = free_list;
  free_list = tmp;
}

uf_node_t*
get_uf_node()
{
  return ( (uf_node_t*) get_node() );
}

void
return_uf_node(uf_node_t* node)
{
  return_node( (tree_node_t*) node );
}

uf_node_t*
uf_insert(item_t* new_item)
{
  uf_node_t* new_node;
  new_node = get_uf_node();
  new_node->item = new_item;
  new_node->height   = 0;
  new_node->indegree = 0;
  new_node->up = NULL;
  new_node->list = NULL;

  return ( new_node );
}

int
same_class(uf_node_t* node1, uf_node_t* node2)
{
  uf_node_t* tmp1, *tmp2;

  /* find both roots */
  for ( tmp1 = node1; tmp1->up != NULL; tmp1 = tmp1->up)
    ; /* follow path to root for node1 */

  for ( tmp2 = node2; tmp2->up != NULL; tmp2 = tmp2->up)
    ; /* follow path to root for node2 */

  /* return result */
  return ( tmp1 == tmp2 );
}

void
join(uf_node_t* node1, uf_node_t* node2)
{
  uf_node_t* root1, *root2, *tmp;
  int i;

  /* find both roots */
  for ( root1 = node1; root1->up != NULL; root1 = root1->up)
    ; /* follow path to root for node1 */

  for ( root2 = node2; root2->up != NULL; root2 = root2->up)
    ; /* follow path to root for node2 */

  if ( root1->height < root2->height ) {
    tmp = root1;
    root1 = root2;
    root2 = tmp;
  }  /* now root1 is the larger subtree */

  if ( root1->height >= 2 ) {

    /* inserting two levels below root 1, height stays the same */
    if ( root2->height < root1->height ) {
      tmp = root2->list;        /* go through list below root2 */

      while ( tmp != NULL ) {
        tmp->up = root1->list; /* point to node on root1 list */
        tmp = tmp->list;
      }
      root2->up = root1->list;/* also point root2 to that node */

    } else { /* root2->height == root1->height */
      /* join root2 list to root1 list, pointing to root1 */
      tmp = root2->list;
      tmp->up = root1;

      while ( tmp->list != NULL ) {
        tmp = tmp->list; /* move to end of root2 list */
        tmp->up = root1;
      }

      tmp->list = root1->list;
      root1->list = root2->list; /* linked lists */
      root1->indegree += root2->indegree;

      /* now lists joined together below root 1 */
      if ( root1->indegree <= root1->height )
      { root2->up = root1->list; } /* point to node on root1 list */

      else { /* root2 becomes new root, root1 goes below */
        root1->up = root2;
        root1->list = NULL;
        root2->height += 1;
        root2->indegree = 1;
        root2->list = root1;
      }
    }

  } else { /* root1->height <= 1*/

    if ( root1->height == 0 ) {
      root1->height = 1;
      root1->indegree = 1;
      root1->list = root2;
      root2->up = root1;   /* root1 is new root */

    } else /* root1->height == 1 */
      /* any root at height 1 has exactly one lower neighbor */

    {
      if ( root2->height == 1 ) /* both height 1 */
      { root2->list->up = root1; }
      /* now make root1 lower neighbor of root2 */
      root2->height = 2;
      root2->indegree = 1;
      root2->list = root1;
      root1->list = NULL;
      root1->up = root2;
      /* now root2 is the new root */
    }
  }
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

    if (  tree->key == delete_key ) {
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

      if ( tmp_node->left->height -
           tmp_node->right->height == 2 ) {

        if ( tmp_node->left->left->height -
             tmp_node->right->height == 1 ) {
          right_rotation( tmp_node );
          tmp_node->right->height =
            tmp_node->right->left->height + 1;
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

        if ( tmp_node->right->right->height -
             tmp_node->left->height == 1 ) {
          left_rotation( tmp_node );
          tmp_node->left->height =
            tmp_node->left->right->height + 1;
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
  printf("Test worst-case optimal union-find structure\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int* new_item;
      uf_node_t* new_uf_node;
      int success;
      new_item = (int*) malloc(sizeof(int));
      scanf(" %d", new_item);
      new_uf_node = uf_insert(new_item);
      success = insert( searchtree, *new_item, new_uf_node );

      if ( success == 0 )
      { printf("  inserted item %d successfully\n", *new_item); }

      else
      { printf("  insert failed, success = %d\n", success); }
    }

    if ( nextop == 's' ) {
      int item1, item2;
      uf_node_t* n1, *n2;
      scanf(" %d %d", &item1, &item2);
      n1 = find( searchtree, item1);
      n2 = find( searchtree, item2);

      if ( same_class( n1, n2) )
      { printf("  items %d and %d are in the same class\n", item1, item2); }

      else
      { printf(" items %d and %d are in distinct classes\n", item1, item2); }
    }

    if ( nextop == 'j' ) {
      int item1, item2;
      uf_node_t* n1, *n2;
      scanf(" %d %d", &item1, &item2);
      n1 = find( searchtree, item1);
      n2 = find( searchtree, item2);
      join( n1, n2 );

      if ( same_class( n1, n2) )
      { printf("  joined classes of items %d and %d\n", item1, item2); }

      else
      { printf("  join of classes %d and %d failed\n", item1, item2); }
    }

    if ( nextop == '?' ) {
      int item;
      uf_node_t* n, *m;
      scanf(" %d", &item);
      printf("looking for node with key %d\n", item);
      n = find( searchtree, item);

      if (n != NULL )
      { printf(" found node %d\n", item ); }

      else
      { printf(" wrong node key\n"); }

      if ( n->up == NULL )
      { printf( " its up-field is NULL \n"); }

      if ( n->list == NULL )
      { printf( " its list-field is NULL \n"); }

      if ( n->up == NULL) {
        printf("The node  of item %d is a root node \n", *(n->item));
        printf("It has height %d and indegree %d\n", n->height, n->indegree);
        printf("The nodes on the list below this node have items\n");
        m = n->list;

        while ( m != NULL ) {
          printf("%d ", *(m->item));

          if ( m->up != n)
          { printf("(error in next!)  "); }

          m = m->list;
        }
        printf(" end\n");

      } else {
        printf("The node  of item %d is a not a root node \n", *(n->item));
        printf("The nodes on the path to the root node have the items\n");
        m = n->up;

        while ( m != NULL ) {
          printf("%d ", *(m->item));
          m = m->up;
        }
        printf(" end\n");
      }
    }
  }

  return (0);
}
