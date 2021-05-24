#include <stdlib.h>
#include <stdio.h>

typedef int key_t;

typedef int object_t;

typedef struct hp_n_t {
  int             rank;
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
      currentblock = (node_t*) malloc( BLOCKSIZE * sizeof(node_t) );
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
  tmp_node->rank = 0;

  return ( tmp_node );
}

int
heap_empty(heap_node_t* hp)
{
  return ( hp->rank == 0 );
}

key_t
find_min_key(heap_node_t* hp)
{
  return ( hp->key );
}

object_t*
find_min_object(heap_node_t* hp)
{
  return ( hp->object );
}

void
remove_heap(heap_node_t* hp)
{
  heap_node_t* current_node, *tmp;
  if ( hp->rank == 0)
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

int
insert(key_t new_key, object_t* new_obj, heap_node_t* hp)
{
  if (hp->rank == 0) { /* insert in empty heap */
    hp->object = new_obj;
    hp->key = new_key;
    hp->left = hp->right = NULL;
    hp->rank = 1;

  } else if ( new_key < hp->key ) { /* new minimum, replace root */

    heap_node_t* tmp;
    tmp = get_node();
    tmp->left   = hp->left;
    tmp->right  = hp->right;
    tmp->key    = hp->key;
    tmp->rank   = hp->rank;
    tmp->object = hp->object;
    hp->left   = tmp;
    hp->right  = NULL;
    hp->key    = new_key;
    hp->object = new_obj;
    hp->rank   = 1;

  } else { /* normal insert */

    heap_node_t* tmp, *tmp2, *new_node;
    heap_node_t* node_stack[100];
    int stack_p = 0;
    tmp = hp;

    /* go down right path to the insertion point */
    while ( tmp->right != NULL && tmp->right->key < new_key) {
      node_stack[stack_p++] = tmp ;
      tmp = tmp->right;
    }

    /* now create new node */
    new_node = get_node();
    new_node->key = new_key;
    new_node->object = new_obj;

    /* insert new node in path, everything below goes left */
    new_node->left  = tmp->right;
    new_node->right = NULL;
    new_node->rank  = 1;

    if ( tmp->left == NULL ) /* possible only at the end */
    { tmp->left  = new_node; } /* here tmp->right == NULL */

    else { /* insert right, restore leftist property */
      tmp->right = new_node;
      tmp->rank  = 2; /* has rank at least one also left */
      /* completed insert, now move up, recompute rank and
         exchange left and right where necessary */

      while ( stack_p > 0 ) {
        tmp = node_stack[--stack_p];
        {
          if (tmp->left->rank < tmp->right->rank ) {
            tmp2 = tmp->left;
            tmp->left = tmp->right;
            tmp->right = tmp2;
          }
          tmp->rank = tmp->right->rank + 1;
        }
      }
    }  /* end walking back to the root */
  }

  return (0);
}

heap_node_t*
merge(heap_node_t* hp1, heap_node_t* hp2)
{
  heap_node_t* root, *tmp1, *tmp2, *tmp3;
  heap_node_t* node_stack[100];
  int stack_p = 0;
  printf(" >merge: started \n");

  if ( hp1->rank == 0 ) { /* heap 1 empty */
    return_node(  hp1 );
    return ( hp2 );
  }

  if ( hp2->rank == 0 ) { /* heap 2 empty */
    return_node(  hp2 );
    return ( hp1 );
  } /* select new root, setup merging */

  if ( hp1->key < hp2->key ) {
    tmp1 = root = hp1;
    tmp2 = hp1->right;
    tmp3 = hp2;

  } else {
    tmp1 = root = hp2;
    tmp2 = hp2->right;
    tmp3 = hp1;
  }

  while ( tmp2 != NULL && tmp3 != NULL ) {

    if ( tmp2->key < tmp3->key ) {
      tmp1->right = tmp2;
      node_stack[stack_p++] = tmp1 ;
      tmp1 = tmp2;
      tmp2 = tmp2->right;

    } else {
      tmp1->right = tmp3;
      node_stack[stack_p++] = tmp1 ;
      tmp1 = tmp3;
      tmp3 = tmp3->right;
    }
  }

  if ( tmp2 == NULL)
  { tmp1->right = tmp3; }

  else
  { tmp1->right = tmp2; }

  /* merging of right paths complete, now recompute rank */
  /* and restore leftist property */
  node_stack[stack_p++] = tmp1 ;

  while ( stack_p > 0 ) {
    tmp1 = node_stack[--stack_p];

    if ( tmp1->left == NULL || (tmp1->left != NULL && tmp1->right != NULL
                                && tmp1->left->rank < tmp1->right->rank ) ) {
      tmp2 = tmp1->left;
      tmp1->left = tmp1->right;
      tmp1->right = tmp2;
    }

    if ( tmp1->right == NULL )
    { tmp1->rank = 1; }

    else
    { tmp1->rank = tmp1->right->rank + 1; }
  }
  return ( root );
}

object_t*
delete_min(heap_node_t* hp)
{
  object_t* del_obj;
  heap_node_t* heap1, *heap2, *tmp;
  del_obj = hp->object;
  heap1 = hp->left;
  heap2 = hp->right;

  if ( heap1 == NULL && heap2 == NULL )
  { hp->rank = 0; }

  else {

    if ( heap2 == NULL )
    { tmp = heap1; }

    else
    { tmp = merge( heap1, heap2); }

    /* now they are merged, need to copy root to correct place*/
    hp->key    = tmp->key;
    hp->object = tmp->object;
    hp->rank   = tmp->rank;
    hp->left   = tmp->left;
    hp->right  = tmp->right;
    return_node( tmp );
  }
  return ( del_obj );
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
      int inskey,  *insobj, success;
      insobj = (object_t*) malloc(sizeof(object_t));
      scanf(" %d,%d", &inskey, insobj);
      success = insert( inskey, insobj, heap );

      if ( success == 0 )
        printf("  insert successful, key = %d, object value = %d, \
current heap rank is %d\n", inskey, *insobj, heap->rank );

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
        printf("  delete successful, deleted object %d, current heap rank is %d\n",
               *delobj, heap->rank );
    }

    if ( nextop == '?' ) {
      heap_node_t* tmp;
      getchar();
      tmp = heap;
      printf(" left path: key values ");

      while ( tmp != NULL )
      { printf(" %d,", tmp->key); tmp = tmp->left; }

      printf("\n");
      tmp = heap;
      printf(" right path: key values ");

      while ( tmp != NULL )
      { printf(" %d,", tmp->key); tmp = tmp->right; }

      printf("\n");
    }
  }
  return (0);
}
