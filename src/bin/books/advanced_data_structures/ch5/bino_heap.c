#include <stdlib.h>
#include <stdio.h>

typedef int key_t;

typedef int object_t;

typedef struct hp_n_t {
  int           height;
  key_t            key;
  object_t*     object;
  struct hp_n_t*  left;
  struct hp_n_t* right;
} heap_node_t;

typedef heap_node_t node_t;

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
    free_list = free_list -> left;

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
  node->left = free_list;
  free_list = node;
}

heap_node_t*
create_heap(void)
{
  heap_node_t* tmp_node;

  tmp_node = get_node();
  tmp_node->height = -1;
  tmp_node->left = tmp_node->right = NULL;

  return ( tmp_node );
}

int
heap_empty(heap_node_t* hp)
{
  return ( hp->height == -1 );
}

key_t
find_min_key(heap_node_t* hp)
{
  heap_node_t* tmp;
  key_t        tmp_key;

  tmp     = hp;
  tmp_key = hp->key;

  while ( tmp->left != NULL ) {
    tmp = tmp->left;

    if ( tmp->key < tmp_key )
    { tmp_key =  tmp->key; }
  }

  return ( tmp_key );
}

void
remove_heap(heap_node_t* hp)
{
  heap_node_t* current_node, *tmp;

  if ( hp->height == 0)
  { return_node( hp ); }

  else {
    current_node = hp;

    while (current_node != NULL ) {

      if ( current_node->left == NULL ) {
        tmp = current_node->right;
        return_node( current_node );
        current_node = tmp;

      } else {
        tmp = current_node;
        current_node = current_node->left;
        tmp->left = current_node->right;
        current_node->right = tmp;
      }
    }
  }
}

heap_node_t*
merge(heap_node_t* hp1, heap_node_t* hp2)
{
  heap_node_t* tmp1, *tmp2, *current, *next;
  heap_node_t* node_stack[100];
  int stack_p = 0;

  if ( hp1->height == -1 ) { /* heap 1 empty */
    return_node(  hp1 );
    return ( hp2 );
  }

  if ( hp2->height == -1 ) { /* heap 2 empty */
    return_node(  hp2 );
    return ( hp1 );
  }

  /* put all the blocks on the stack */
  tmp1 = hp1;
  tmp2 = hp2;

  while ( tmp1 != NULL && tmp2 != NULL ) {

    if ( tmp1->height > tmp2->height ) {
      node_stack[ stack_p++ ] = tmp1;
      tmp1 = tmp1->left;

    } else {
      node_stack[ stack_p++ ] = tmp2;
      tmp2 = tmp2->left;
    }
  }

  /* one list is empty, push the rest of the other */
  while ( tmp1 != NULL ) {
    node_stack[ stack_p++ ] = tmp1;
    tmp1 = tmp1->left;
  }

  while ( tmp2 != NULL ) {
    node_stack[ stack_p++ ] = tmp2;
    tmp2 = tmp2->left;
  }

  /* now all the blocks are on the stack */
  /* put them together, performing addition */
  current = node_stack[ --stack_p ];
  while ( stack_p > 0 ) {
    next = node_stack[ --stack_p ];

    if ( next->height > current->height ) {
      next->left = current; /* add in front of left list */
      current = next;

    } else if ( next->height == current->height ) { /*add blocks*/

      if ( next->key < current->key ) {
        next->left = current->left;
        current->left = next->right;
        next->right = current;
        next->height += 1;
        current = next;

      } else {
        next->left = current->right;
        current->right = next;
        current->height += 1;
      }

    } else { /* next->height < current->height */

      next->left = current->left; /* exchange current, next*/
      current->left = next; /* insert next just below current*/
    }
  }
  return ( current );
}

object_t*
delete_min(heap_node_t* hp)
{
  object_t* del_obj;
  heap_node_t* tmp1, *tmp2, *min1, *min2;
  key_t tmp_key;

  if ( hp->height == 0 ) { /* delete last object, heap now empty */
    hp->height = -1;
    return ( hp->object );
  } /* can assume now heap will not become empty */

  tmp1 = tmp2 = hp;
  tmp_key  = hp->key;
  min1 = min2 = hp;

  while ( tmp1->left != NULL ) {
    tmp2 = tmp1; /* tmp2 node above tmp1 on left path */
    tmp1 = tmp1->left;

    if ( tmp1->key < tmp_key) {
      tmp_key =  tmp1->key;     /* min1 is minimum node */
      min1 = tmp1;
      min2 = tmp2; /* min2 node above min1 */
    }
  }

  del_obj = min1->object;

  if ( min1 != min2 ) { /* min1 not root, so node above exists */
    min2->left = min1->left; /* unlinked min1 */

    if ( min1->height > 0 ) { /* min1 has right subtree */
      tmp1 = min1->right;  /* save its right tree */
      min1->key    = hp->key; /* copy root into min1 */
      min1->object = hp->object;
      min1->height = hp->height;
      min1->left   = hp->left;
      min1->right  = hp->right;
      tmp2 = merge( min1, tmp1 ); /* and merge */

    } else { /* min1 is leaf on left path */
      return_node( min1 );
      return ( del_obj );
    }
  }  /* min1 is root node, has left and right subtree */

  else if ( min1->left != NULL )
  { tmp2 = merge(  min1->left, min1->right ); }

  else /* min1 is root node, has only right subtree */
  { tmp2 = min1->right; }

  /* merge completed, now copy new root back */
  hp->key    = tmp2->key;
  hp->object = tmp2->object;
  hp->height = tmp2->height;
  hp->left   = tmp2->left;
  hp->right  = tmp2->right;
  return_node( tmp2 );

  return ( del_obj );
}

int
insert(key_t new_key, object_t* new_obj, heap_node_t* hp)
{
  heap_node_t* new_node, *tmp, *tmp2;

  new_node = get_node(); /* create one-element heap */
  new_node->height = 0;
  new_node->key    = new_key;
  new_node->object = new_obj;
  new_node->left = new_node->right = NULL;

  tmp = get_node(); /* copy root into tmp_node */
  tmp->left   = hp->left;
  tmp->right  = hp->right;
  tmp->key    = hp->key;
  tmp->object = hp->object;
  tmp->height   = hp->height;
  tmp2 = merge( new_node, tmp ); /* merge the heaps */

  hp->left = tmp2->left; /* merge completed, copy root back */
  hp->right = tmp2->right;
  hp->key = tmp2->key;
  hp->object = tmp2->object;
  hp->height = tmp2->height;

  return_node( tmp2 );
  return ( 0 );
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  heap_node_t* heap;
  char nextop;
  heap = create_heap();
  printf("Made Heap\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int inskey, *insobj, success;
      insobj = (object_t*) malloc(sizeof(object_t));
      scanf(" %d,%d", &inskey, insobj);
      success = insert( inskey, insobj, heap );

      if ( success == 0 )
        printf("  insert successful, key = %d, object value = %d, \
current heap height is %d\n", inskey, *insobj, heap->height );

      else
      { printf("  insert failed, success = %d\n", success); }
    }

    if ( nextop == 'd' ) {
      object_t* delobj;
      getchar();
      delobj = delete_min( heap);

      if ( delobj == NULL )
      { printf("  delete failed\n"); }

      else
        printf("  delete successful, deleted object %d, current heap height is %d\n",
               *delobj, heap->height );
    }

    if ( nextop == '?' ) {
      heap_node_t* tmp;
      getchar();
      tmp = heap;
      printf(" left path: key, height values ");

      while ( tmp != NULL )
      { printf(" %d,%d |", tmp->key, tmp->height); tmp = tmp->left; }

      printf("\n");
      tmp = heap;
      printf(" right path: key, height values ");

      while ( tmp != NULL )
      { printf(" %d,%d |", tmp->key, tmp->height); tmp = tmp->right; }

      printf("\n");

    }
  }
  return (0);
}
