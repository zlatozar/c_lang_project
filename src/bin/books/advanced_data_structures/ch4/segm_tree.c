#include <stdio.h>
#include <stdlib.h>

struct intv {
  int low;
  int up;
};

typedef struct intv object_t;

typedef int key_t;

typedef struct ls_n_t {
  key_t          key_a, key_b;
  struct ls_n_t*         next;
  object_t*            object;
} list_node_t;

typedef struct tr_n_t {
  key_t                  key;
  struct tr_n_t*        left;
  struct tr_n_t*       right;
  list_node_t* interval_list;
  /* balancing  information */
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

list_node_t*
get_list_node()
{
  return ( ( list_node_t*) get_node() );
}


tree_node_t*
make_tree(tree_node_t* list)
{
  typedef struct {
    tree_node_t*  node1;
    tree_node_t*  node2;
    int          number;
  } st_item;

  st_item current, left, right;
  st_item stack[100];
  int st_p = 0;
  tree_node_t* tmp, *root;
  int length = 0;

  for ( tmp = list; tmp != NULL; tmp = tmp->right )
  { length += 1; } /* find length of list */

  root = get_node();
  current.node1 = root; /* put root node on stack */
  current.node2 = NULL;
  current.number = length;/* root expands to length leaves */
  stack[ st_p++ ] = current;

  while ( st_p > 0 ) { /* there is still unexpanded node */
    current = stack[ --st_p ];

    if ( current.number > 1 ) { /* create (empty) tree nodes */
      left.node1 = get_node();
      left.node2 = current.node2;
      left.number = current.number / 2;

      right.node1 = get_node();
      right.node2 = current.node1;
      right.number = current.number - left.number;

      (current.node1)->left  = left.node1;
      (current.node1)->right = right.node1;

      stack[ st_p++ ] = right;
      stack[ st_p++ ] = left;

    } else { /* reached a leaf, must be filled with list item */
      (current.node1)->left  = list->left;   /* fill leaf */
      (current.node1)->key   = list->key;    /* from list */
      (current.node1)->right = NULL;

      if ( current.node2 != NULL )
        /* insert comparison key in interior node */
      { (current.node2)->key   = list->key; }

      tmp = list;          /* unlink first item from list */
      list = list->right;  /* content has been copied to */
      return_node(tmp);    /* leaf, so node is returned */
    }
  }

  return ( root );
}

void
empty_tree(tree_node_t* stree)
{
  stree->interval_list  = NULL;

  if ( stree->right != NULL ) {
    empty_tree( stree->left );
    empty_tree( stree->right );
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

    if ( *( (int*) tr->left) == 42 )
    { printf("%d(%d)  ", tr->key, depth ); }

    else
    { printf("Wrong Object \n"); }

  } else {

    check_tree(tr->left, depth + 1, lower, tr->key );
    check_tree(tr->right, depth + 1, tr->key, upper );
  }
}

list_node_t*
find_intervals(tree_node_t* tree, key_t query_key)
{
  tree_node_t* current_tree_node;
  list_node_t* current_list, *result_list, *new_result;

  if ( tree->left == NULL ) /* tree empty */
  { return (NULL); }

  else { /* tree nonempty, follow search path */
    current_tree_node = tree;
    result_list = NULL;

    while ( current_tree_node->right != NULL ) {

      if ( query_key < current_tree_node->key )
      { current_tree_node = current_tree_node->left; }

      else
      { current_tree_node = current_tree_node->right; }

      current_list = current_tree_node->interval_list;
      while ( current_list != NULL ) {
        /* copy entry from node list to result list */
        new_result = get_list_node();
        new_result->next = result_list;
        new_result->key_a = current_list->key_a;
        new_result->key_b = current_list->key_b;
        new_result->object = current_list->object;
        result_list = new_result;
        current_list = current_list->next;
      }
    }
    return ( result_list );
  }
}

void
attach_intv_node(tree_node_t* tree_node, key_t a, key_t b, object_t* object)
{
  list_node_t* new_node;

  new_node = get_list_node();
  new_node->next = tree_node->interval_list;
  new_node->key_a = a;
  new_node->key_b = b;
  new_node->object = object;

  tree_node->interval_list = new_node;
}

void
insert_interval(tree_node_t* tree, key_t a, key_t b, object_t* object)
{
  tree_node_t* current_node, *right_path, *left_path;
  list_node_t* current_list, *new_node;

  if ( tree->left == NULL )
  { exit(-1); } /* tree incorrect */

  else {
    current_node = tree;
    right_path = left_path = NULL;

    while ( current_node->right != NULL ) { /* not at leaf */

      if ( b < current_node->key ) /* go left: a < b < key */
      { current_node = current_node->left; }

      else if ( current_node->key < a)
        /* go right: key < b < a */
      { current_node = current_node->right; }

      else if ( a < current_node->key && current_node->key < b ) { /* split: a < key < b */
        right_path = current_node->right;  /* both right */
        left_path  = current_node->left;   /* and left */
        break;

      } else if ( a == current_node->key ) {  /* a = key < b */
        right_path = current_node->right;     /* no left */
        break;

      } else {  /* current_node->key == b, so a < key = b */
        left_path = current_node->left; /* no right */
        break;
      }
    }

    if ( left_path != NULL ) {

      /* now follow the path of the left endpoint a*/
      while ( left_path->right != NULL ) {

        if ( a < left_path->key ) {
          /* right node must be selected */
          attach_intv_node(left_path->right, a, b, object);
          left_path = left_path->left;

        } else if ( a == left_path->key ) {
          attach_intv_node(left_path->right, a, b, object);
          break; /* no further descent necessary */

        } else /* go right, no node selected */
        { left_path = left_path->right; }
      }

      /* left leaf needs to be selected if reached in descent*/
      if ( left_path->right == NULL && left_path->key == a )
      { attach_intv_node(left_path, a, b, object); }
    }  /* end left path */

    if ( right_path != NULL ) {

      /* and now follow the path of the right endpoint b */
      while ( right_path->right != NULL ) {

        if ( right_path->key < b ) {
          /* left node must be selected */
          attach_intv_node(right_path->left, a, b, object);
          right_path = right_path->right;

        } else if ( right_path->key == b) {

          attach_intv_node(right_path->left, a, b, object);
          break; /* no further descent necessary */

        } else /* go left, no node selected */
        { right_path = right_path->left; }
      }
      /* on the right side, the leaf of b is never attached */
    } /* end right path */
  }
}

int
compint(int* a, int* b)
{
  return ( *a > *b );
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  tree_node_t* seg_tree, *tmp, *list;
  char nextop;
  struct intv intervals[100];
  int keys[200];
  int prev_key;
  int i = 0, j;
  int* tmpob;

  printf("Please enter the intervals\n");
  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int lower, upper;
      scanf(" %d %d", &lower, &upper);
      intervals[i].low = lower;
      intervals[i].up  = upper;
      keys[2 * i] = lower;
      keys[2 * i + 1] = upper;
      i += 1;
    }
  }

  printf("stored %d intervals \n", i);
  printf("  the %d intervals, are \n", i);

  for ( j = 0; j < i; j++ )
  { printf("[%d,%d[ ", intervals[j].low, intervals[j].up ); }

  printf("\n");
  qsort(keys, 2 * i, sizeof( int ), compint );
  printf("  the keys, sorted in increasing order, are \n");

  for ( j = 0; j < 2 * i; j++ )
  { printf("%d, ", keys[j] ); }

  printf("\n");
  list = get_node();
  list->right = NULL;

  prev_key = list->key = keys[2 * i - 1];

  tmpob  = (int*) malloc(sizeof(int));
  *tmpob = 42;
  list->left = (tree_node_t*) tmpob;

  for ( j = 2 * i - 2; j >= 0; j-- ) {
    if ( keys[j] != prev_key ) {
      tmp = get_node();
      prev_key = tmp->key = keys[j];
      tmp->right = list;
      tmpob  = (int*) malloc(sizeof(int));
      *tmpob = 42;
      tmp->left = (tree_node_t*) tmpob;
      list = tmp;
    }
  }

  tmp = get_node();
  tmp->key = -1000;
  tmp->right = list;
  tmpob  = (int*) malloc(sizeof(int));
  *tmpob = 42;
  tmp->left = (tree_node_t*) tmpob;
  list = tmp;
  printf("Built sorted list from keys\n");
  tmp = list;

  while (tmp != NULL ) {
    printf("%d ", tmp->key );
    tmp = tmp->right;
  }
  printf("\n");
  seg_tree = make_tree( list );
  printf("Made Tree\n");
  empty_tree(seg_tree);
  check_tree(seg_tree, 0, -2000, 1000);
  printf(" Tree tested \n");

  for (  j = i - 1; j >= 0; j-- ) {
    struct intv* tmp_ob;
    tmp_ob = (struct intv*) malloc( sizeof( struct intv ) );
    tmp_ob->low = intervals[j].low;
    tmp_ob->up = intervals[j].up;
    insert_interval( seg_tree, intervals[j].low, intervals[j].up, tmp_ob );
  }

  printf("inserted the intervals into the tree. Now entering query phase\n");
  while ( (nextop = getchar()) != 'q' ) {
    list_node_t* answer_list;

    if ( nextop == 'f' ) {
      int findkey;
      scanf(" %d", &findkey);
      answer_list = find_intervals( seg_tree, findkey );

      if ( answer_list == NULL )
      { printf("  no interval found for key %d\n", findkey); }

      else
      { printf("  the following intervals contain key %d\n", findkey); }

      while ( answer_list != NULL ) {
        printf(" [%d,%d[ ", answer_list->object->low,
               answer_list->object->up);
        answer_list = answer_list->next;
      }
      printf(" end\n");
    }
  }
  return (0);
}
