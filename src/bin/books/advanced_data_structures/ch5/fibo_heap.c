#include <stdlib.h>
#include <stdio.h>

typedef int key_t;

typedef int object_t;

typedef struct hp_n_t {
  key_t            key;
  object_t*     object;
  struct hp_n_t*  left;
  struct hp_n_t* right;
  struct hp_n_t*    up;
  struct hp_n_t*  r_up;
  int             rank;
  enum {complete, deficient} state;
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
  tmp_node->right = NULL;
  return ( tmp_node );
}

int
heap_empty(heap_node_t* hp)
{
  return ( hp->right == NULL );
}

key_t
find_min_key(heap_node_t* hp)
{
  return ( hp->left->key);
}

heap_node_t*
insert(key_t new_key, object_t* new_obj, heap_node_t* hp)
{
  heap_node_t* new_node;

  new_node = get_node(); /* create new node */
  new_node->right = NULL;
  new_node->key = new_key;
  new_node->object = new_obj;
  new_node->rank = 0;
  new_node->state = complete;

  if ( hp->right == NULL ) { /* insert in empty heap */
    hp->right = hp->left = hp->up = new_node;
    new_node->left = NULL;

  } else { /* heap nonempty, put on top of leftmost path */
    new_node->left = hp->right;
    hp->right = new_node;
    new_node->left->up = new_node;

    if ( hp->left->key > new_key) /* update min-pointer */
    { hp->left = new_node; }
  }

  return ( new_node );
}

heap_node_t*
merge(heap_node_t* hp1, heap_node_t* hp2)
{
  if ( hp1->right == NULL ) {   /* hp1 empty */
    return_node(hp1);
    return (hp2);

  } else if ( hp2->right == NULL ) { /* hp2 empty */
    return_node(hp2);
    return (hp1);

  } else { /* both heaps nonempty */
    hp1->up->left = hp2->right; /* concatenate leftmost paths */
    hp2->right->up = hp1->up;  /* join their up-pointers */
    hp1->up = hp2->up;        /* restore leftend pointer */

    if (hp1->left->key > hp2->left->key)
    { hp1->left = hp2->left; } /* update min pointer */

    return_node(hp2);
    return (hp1);
  }
}

void
decrease_key(key_t new_key, heap_node_t* n, heap_node_t* hp)
{
  heap_node_t* u, *tmp;
  int finished = 0;
  n->key = new_key; /* decrease key in n */

  if ( new_key < hp->left->key ) /* update min-pointer */
  { hp->left = n; }

  while ( n->r_up != NULL && !finished ) {

    u = n->r_up; /* n on left path of u->right: unlink n */
    if ( n == u->right ) { /* n on top of left path of u->right */
      u->right = n->left;

      if ( n->left != NULL )
      { n->left->up = u; }

    } else { /* n further down on left path of u->right */
      n->up->left = n->left;

      if ( n->left != NULL )
      { n->left->up = n->up; }

    } /* unlink n complete, now insert n on leftmost path */

    n->r_up = NULL;
    n->left = hp->right;
    n->left->up = n;

    hp->right = n;
    /* now repair u; if necessary, repeat cut */
    if ( u->r_up == NULL ) { /* u already on leftmost path*/
      u->rank -= 1;
      finished = 1;

    } else if ( u->state == complete ) { /* u becomes deficient*/
      u->state = deficient;
      finished = 1;

    } else { /* u deficient and not on leftmost path*/

      if ( u->rank >= 2 )
      { u->rank -= 2; }

      else
      { u->rank = 0; }
      u->state = complete; /* u rank information correct */
    } /* in this case, have to cut u from left list */

    n = u; /* so repeat unlink operation */

  }/* end of while loop, finished with `cascading cut' */
}

object_t*
delete_min(heap_node_t* hp)
{
  heap_node_t* min, *tmp, *tmp2;
  object_t* del_obj;
  heap_node_t* rank_class[100];
  int i;
  key_t tmp_min;

  if ( hp->right == NULL) /* heap empty, delete failed */
  { return ( NULL ); }

  min = hp->left; /* unlink min node from leftmost path */
  del_obj = min->object;

  if ( min == hp->right ) { /* min on top of leftmost path */

    if ( min->left != NULL ) { /* path continues after min */
      hp->right = min->left;
      min->left->up = hp;

    } else { /* min only vertex on leftmost path */

      if ( min->right != NULL ) { /* min not last node */
        hp->right = min->right;
        min->right->up = hp;
        min->right = NULL;

      } else { /* min last node, heap now empty */
        hp->right = NULL;
        return_node( min );
        return ( del_obj );
      }
    }

  } else { /* min further down on leftmost path */
    min->up->left = min->left;
    if ( min->left != NULL ) /* min not last vertex */
    { min->left->up = min->up; }

  } /* unlink min complete */

  /* now move left path of min->right to leftmost path */
  if ( min->right != NULL ) { /* path nonempty */
    tmp = min->right;

    while ( tmp->left != NULL ) /* find end of path */
    { tmp = tmp->left; }

    tmp->left = hp->right;
    tmp->left->up = tmp;
    hp->right = min->right;
    min->right->up = hp;
  }

  /* now path below min->right linked to leftmost path */
  return_node( min ); /* minimum deleted */
  /* now starts clean-up phase */

  for ( i = 0; i < 100; i++)
  { rank_class[i] = NULL; }

  /* now unbuild leftmost path, collect nodes of equal rank*/
  tmp = hp->right;  /* take first node from leftmost path */
  hp->right = hp->right->left; /* unlink that node */

  while ( tmp != NULL ) {

    if ( rank_class[tmp->rank] == NULL ) {
      /* no node of same rank found: store node */
      rank_class[tmp->rank] = tmp;
      tmp = hp->right; /* take new node */

      if ( tmp != NULL)
      { hp->right = hp->right->left; } /* unlink that node */

    } else { /* two nodes of same rank found, add blocks */

      tmp2 = rank_class[tmp->rank];
      rank_class[tmp->rank] = NULL;

      if ( tmp->key < tmp2->key ) {
        tmp2->left = tmp->right;
        tmp->right = tmp2;

      } else { /* tmp->key >= tmp2->key */
        tmp->left = tmp2->right;
        tmp2->right = tmp;
        tmp = tmp2;
      }

      tmp->rank += 1; /* increase rank of sum block */
    }
  } /* all remaining blocks now in rank_class[] */
  /* now rebuild the leftmost path */

  hp->right = NULL;

  for ( i = 0; i < 100; i++) {

    if ( rank_class[i] != NULL ) {
      tmp = rank_class[i];
      tmp->left = hp->right;
      hp->right = tmp;
    }
  }

  /* recompute pointers on new leftmost path */
  hp->left = hp->right;
  tmp_min = hp->left->key;

  for ( tmp = hp->right; tmp->left != NULL; tmp = tmp->left) {
    tmp->left->up = tmp;     /* new up pointers */

    if ( tmp->left->key < tmp_min ) {
      hp->left = tmp->left; /* new min pointer */
      tmp_min = tmp->left->key;
    }
  }

  hp->up = tmp; /* end of leftmost path */
  /* finished with clean-up phase */
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
      int inskey,  *insobj;
      heap_node_t* success;
      insobj = (object_t*) malloc(sizeof(object_t));
      scanf(" %d,%d", &inskey, insobj);
      success = insert( inskey, insobj, heap );

      if ( success != NULL )
      { printf("  insert successful, key = %d, object value = %d,\n", inskey, *insobj); }

      else
      { printf("  insert failed\n"); }
    }

    if ( nextop == 'd' ) {
      object_t* delobj;
      getchar();
      delobj = delete_min( heap);

      if ( delobj == NULL )
      { printf("  delete failed\n"); }

      else
        printf("  delete successful, deleted object %d,\n",
               *delobj);
    }

    if ( nextop == '?' ) {
      heap_node_t* tmp;
      getchar();
      tmp = heap->right;
      printf(" left path: key, rank values ");

      while ( tmp != NULL )
      { printf(" %d,%d |", tmp->key, tmp->rank); tmp = tmp->left; }

      printf("\n");
      tmp = heap->right;
      printf(" right path: key, rank values ");

      while ( tmp != NULL )
      { printf(" %d,%d |", tmp->key, tmp->rank); tmp = tmp->right; }

      printf("\n");

    }
  }
  return (0);
}
