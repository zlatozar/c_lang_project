#include <stdio.h>
#include <stdlib.h>

struct intv {
  int low;
  int up;
};

typedef struct intv object_t;

typedef int key_t;

typedef struct ls_n_t {
  key_t            key;
  struct ls_n_t*  next;
  object_t*     object;
} list_node_t;

typedef struct tr_n_t {
  key_t               key;
  struct tr_n_t*     left;
  struct tr_n_t*    right;
  list_node_t*  left_list;
  list_node_t* right_list;
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

list_node_t*
find_intervals(tree_node_t* tree, key_t query_key)
{
  tree_node_t* current_tree_node;
  list_node_t* current_list, *result_list, *new_result;

  if ( tree->left == NULL )
  { return (NULL); }

  else {
    current_tree_node = tree;
    result_list = NULL;

    while ( current_tree_node->right != NULL ) {

      if ( query_key < current_tree_node->key ) {
        current_list = current_tree_node->left_list;

        while ( current_list != NULL
                && current_list->key <= query_key ) {
          new_result = get_list_node();
          new_result->next = result_list;
          new_result->object = current_list->object;
          result_list = new_result;
          current_list = current_list->next;
        }
        current_tree_node = current_tree_node->left;

      } else {
        current_list = current_tree_node->right_list;

        while ( current_list != NULL
                && current_list->key >= query_key ) {
          new_result = get_list_node();
          new_result->next = result_list;
          new_result->object = current_list->object;
          result_list = new_result;
          current_list = current_list->next;
        }
        current_tree_node = current_tree_node->right;
      }
    }
    return ( result_list );
  }
}

int
complow(struct intv* a, struct intv* b)
{
  return ((*a).low > (*b).low);
}

int
compup( struct intv* a, struct intv* b )
{
  return ( (*a).up > (*b).up );
}

int
compint( int* a, int* b )
{
  return ( *a > *b );
}

void
empty_tree(tree_node_t* itree)
{
  itree->left_list  = NULL;
  itree->right_list = NULL;

  if ( itree->right != NULL ) {
    empty_tree( itree->left );
    empty_tree( itree->right );
  }
}

void
left_insert_interval(tree_node_t* itree, int lower, int upper)
{
  tree_node_t* tmp;
  list_node_t* tmp_l;
  int node_lower, node_upper;

  struct intv* tmp_i;

  tmp = itree;

  while ( tmp->right != NULL ) {

    if ( upper <= tmp->key )
    { tmp = tmp->left; }

    else if ( tmp->key < lower)
    { tmp = tmp->right; }

    else { /* lower <= tmp->key < upper */
      tmp_l = get_list_node();
      tmp_l->key = lower;
      tmp_i = (struct intv*) malloc ( sizeof( struct intv ));
      tmp_i->low = lower;
      tmp_i->up  = upper;
      tmp_l->object = tmp_i;
      tmp_l->next = tmp->left_list;
      tmp->left_list = tmp_l;
      printf(" inserted interval %d, %d left in node with key %d\n", lower, upper, tmp->key );

      break;
    }
  }
}

void
right_insert_interval(tree_node_t* itree, int lower, int upper)
{
  tree_node_t* tmp;
  list_node_t* tmp_l;
  int node_lower, node_upper ;

  struct intv* tmp_i;

  tmp = itree;
  while ( tmp->right != NULL ) {

    if ( upper <= tmp->key )
    { tmp = tmp->left; }

    else if ( tmp->key < lower)
    { tmp = tmp->right; }

    else { /* lower <= tmp->key < upper */
      tmp_l = get_list_node();
      tmp_l->key = upper;
      tmp_i = (struct intv*) malloc ( sizeof( struct intv ));
      tmp_i->low = lower;
      tmp_i->up  = upper;
      tmp_l->object = tmp_i;
      tmp_l->next = tmp->right_list;
      tmp->right_list = tmp_l;
      printf(" inserted interval %d, %d right in node with key %d\n", lower, upper, tmp->key );

      break;
    }
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

    if ( *( (int*) tr->left) == 42 )
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
  tree_node_t* itree, *tmp, *list;
  char nextop;
  struct intv intervalsA[100], intervalsB[100];
  int keys[200];
  int prev_key;
  int i = 0, j;
  int* tmpob;
  printf("Please enter the intervals\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int lower, upper;
      scanf(" %d %d", &lower, &upper);
      intervalsA[i].low = intervalsB[i].low = lower;
      intervalsA[i].up = intervalsB[i].up = upper;
      keys[2 * i] = lower;
      keys[2 * i + 1] = upper;
      i += 1;
    }
  }

  printf("stored %d intervals \n", i);
  printf("  the %d intervals, are \n", i);

  for ( j = 0; j < i; j++ )
  { printf("[%d,%d[ ", intervalsA[j].low, intervalsA[j].up ); }

  qsort(intervalsA, i, sizeof(struct intv), complow );
  printf("  the %d intervals, sorted according to their lower endpoint are \n", i);

  for ( j = 0; j < i; j++ )
  { printf("[%d,%d[ ", intervalsA[j].low, intervalsA[j].up ); }

  printf("\n");

  qsort(intervalsB, i, sizeof(struct intv), compup );
  qsort(keys, 2 * i, sizeof( int ), compint );
  printf("  the intervals, sorted according to their upper endpoint are \n");

  for ( j = 0; j < i; j++ )
  { printf("[%d,%d[ ", intervalsB[j].low, intervalsB[j].up ); }

  printf("\n");
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

  printf("Built sorted list from keys\n");
  tmp = list;

  while (tmp != NULL ) {
    printf("%d ", tmp->key );
    tmp = tmp->right;
  }

  printf("\n");
  itree = make_tree( list );
  printf("Made Tree\n");
  empty_tree(itree);
  check_tree(itree, 0, -1000, 1000);
  printf(" Tree tested \n");

  for (  j = i - 1; j >= 0; j-- )
  { left_insert_interval( itree, intervalsA[j].low, intervalsA[j].up ); }

  for (  j = i - 1; j >= 0; j-- )
  { right_insert_interval( itree, intervalsB[j].low, intervalsB[j].up ); }

  printf("inserted the intervals into the tree. Now entering query phase\n");

  while ( (nextop = getchar()) != 'q' ) {

    list_node_t* answer_list;

    if ( nextop == 'f' ) {
      int findkey;
      scanf(" %d", &findkey);
      answer_list = find_intervals( itree, findkey );

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
    /* if( nextop == '?' )
    {  printf("  Checking tree\n");
       check_tree(searchtree,0,-1000,1000);
       printf("\n");
       if( searchtree->left != NULL )
    printf("key in root is %d, height of tree is %d\n",
    searchtree->key, searchtree->height );
       printf("  Finished Checking tree\n");
    }
    */
  }
  return (0);
}
