#include <stdlib.h>
#include <stdio.h>

typedef int object_t;

typedef int key_t;

typedef struct tr_n_t {
  key_t        key;
  struct tr_n_t*  next;
  struct tr_n_t*  down;
} tree_node_t;

typedef tree_node_t node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256

node_t* currentblock = NULL;
int size_left;
node_t* free_list = NULL;

node_t*
get_node()
{
  node_t* tmp;

  if ( free_list != NULL ) {
    tmp = free_list;
    free_list = free_list -> next;

  } else {

    if ( currentblock == NULL || size_left == 0) {
      currentblock =
        (node_t*) malloc( BLOCKSIZE * sizeof(node_t) );
      size_left = BLOCKSIZE;
    }

    tmp = currentblock++;
    size_left -= 1;
  }

  return ( tmp );
}

void
return_node(node_t* node)
{
  node->next = free_list;
  free_list = node;
}

tree_node_t*
create_tree(void)
{
  tree_node_t* tree;

  tree = get_node();
  tree->next = NULL;
  tree->down = NULL;

  return ( tree );
}

object_t*
find(tree_node_t* tree, key_t query_key)
{
  tree_node_t* current_node;
  int beyond_placeholder = 0;

  if ( tree->next == NULL ) /* empty skip list */
  { return (NULL); }

  else {
    current_node = tree;

    while ( current_node->down != NULL ) {

      while ( current_node->next != NULL && current_node->next->key <= query_key ) {
        current_node = current_node->next;
        beyond_placeholder = 1;
      }

      current_node = current_node->down;
    }

    if ( beyond_placeholder && current_node->key == query_key )
    { return ( (object_t*) current_node->next ); }

    else
    { return ( NULL ); }
  }
}

int
insert(tree_node_t* tree, key_t new_key, object_t* new_object)
{
  tree_node_t* current_node, *new_node, *tmp_node;
  int max_level, current_level, new_node_level;

  /* create downward list for new node */
  {
    new_node = get_node();
    new_node->key  = new_key;
    new_node->down = NULL;
    new_node->next = (tree_node_t*) new_object;
    new_node_level = 0;

    do {
      tmp_node = get_node();
      tmp_node->down = new_node;
      tmp_node->key = new_key;
      new_node = tmp_node;
      new_node_level += 1;

    } while ( (rand() % 2) < 1 ); /* random choice, probability P */
  }

  tmp_node = tree; /* find the current maximum level */
  max_level = 0;

  while ( tmp_node->down != NULL ) {
    tmp_node = tmp_node->down;
    max_level += 1;
  }

  while ( max_level < new_node_level ) { /* no entry point */
    tmp_node = get_node();
    tmp_node->down = tree->down;
    tmp_node->next = tree->next;
    tree->down = tmp_node;
    tree->next = NULL;
    max_level += 1;
  }

  {  /* find place and insert at all relevant levels */
    current_node = tree;
    current_level = max_level;

    while ( current_level >= 1 ) {

      while ( current_node->next != NULL && current_node->next->key < new_key )
      { current_node = current_node->next; }

      if ( current_level <= new_node_level ) {
        new_node->next = current_node->next;
        current_node->next = new_node;
        new_node = new_node->down;
      }

      if ( current_level >= 2 )
      { current_node = current_node->down; }

      current_level -= 1;
    }
  }
  return ( 0 );
}

object_t*
delete (tree_node_t* tree, key_t delete_key)
{
  tree_node_t* current_node, *tmp_node;
  object_t* deleted_object = NULL;
  current_node = tree;

  while ( current_node->down != NULL ) {

    while ( current_node->next != NULL && current_node->next->key < delete_key )
    { current_node = current_node->next; }

    if ( current_node->next != NULL
         && current_node->next->key == delete_key ) {

      tmp_node = current_node->next; /*unlink node */
      current_node->next = tmp_node->next;

      if ( tmp_node->down->down == NULL ) { /* delete leaf */
        deleted_object = (object_t*) tmp_node->down->next;
        return_node( tmp_node->down );
      }

      return_node( tmp_node );
    }

    current_node = current_node->down;
  }

  /* remove empty levels in placeholder */
  while ( tree->down != NULL  && tree->next == NULL ) {
    tmp_node = tree->down;
    tree->down = tmp_node->down;
    tree->next = tmp_node->next;
    return_node( tmp_node );
  }

  return ( deleted_object );
}

// _____________________________________________________________________________
//                                                                 Simple test

int
main()
{
  tree_node_t* searchtree;
  char nextop;
  searchtree = create_tree();

  printf("Made Tree\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int inskey,  *insobj, success;
      insobj = (int*) malloc(sizeof(int));
      scanf(" %d,%d", &inskey, insobj);

      success = insert( searchtree, inskey, insobj );

      if ( success == 0 )
        printf("  insert successful, key = %d, object value = %d\n",
               inskey, *insobj);

      else
      { printf("  insert failed, success = %d\n", success); }
    }

    if ( nextop == 'f' ) {
      int findkey, *findobj;
      scanf(" %d", &findkey);
      findobj = find( searchtree, findkey);

      {
        tree_node_t* tmp_node;
        int max_level;
        tmp_node = searchtree;
        max_level = 0;

        while ( tmp_node->down != NULL ) {
          tmp_node = tmp_node->down;
          max_level += 1;
        }

        printf("  current skiplist has level %d\n", max_level);
      }

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
      { printf("  delete successful, deleted object %d\n", *delobj); }
    }
  }

  return (0);
}
