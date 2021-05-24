#include <stdio.h>
#include <stdlib.h>

typedef int key_t;

typedef int object_t;

typedef struct qu_t {
  key_t          key;
  object_t*   object;
  struct qu_t*  next;
  struct qu_t*  prev;
} queue_t;

typedef queue_t node_t ;

// _____________________________________________________________________________

#define BLOCKSIZE 256
#define POSINFTY 10000

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

queue_t*
create_minqueue()
{
  queue_t* entrypoint;
  entrypoint = get_node();

  /* create empty object queue below entrypoint->next */
  entrypoint->next = get_node();
  entrypoint->next->next = entrypoint->next;
  entrypoint->next->prev = entrypoint->next;

  /* create empty minkey queue below entrypoint->prev */
  entrypoint->prev = get_node();
  entrypoint->prev->next = entrypoint->prev;
  entrypoint->prev->prev = entrypoint->prev;

  /* minimum over empty set is +infty */
  entrypoint->prev->key = POSINFTY;

  /* empty minqueue created */
  return ( entrypoint );
}

int
queue_empty(queue_t* qu)
{
  return ( qu->next->next == qu->next );
}

key_t
find_min_key(queue_t* qu)
{
  return ( qu->prev->prev->key );
}

object_t*
find_min_obj(queue_t* qu)
{
  return ( qu->prev->prev->object );
}

void
enqueue(object_t* new_obj, key_t new_key, queue_t* qu)
{
  queue_t* new, *tmp;
  tmp = NULL;
  /* create and fill new node with new object and key */
  new = get_node();
  new->object = new_obj;
  new->key = new_key;

  /* insert node in rear of object queue, as qu->next->next */
  new->prev = qu->next;
  qu->next->next->prev = new;
  new->next = qu->next->next;
  qu->next->next = new;

  /* remove all larger keys from rear of minkey queue */
  while ( qu->prev->next != qu->prev &&
          qu->prev->next->key > new_key) {

    if ( tmp != NULL ) /* return node only if we get another*/
    { return_node( tmp ); }

    tmp = qu->prev->next;  /* now unlink tmp */
    qu->prev->next = tmp->next;
    qu->prev->next->prev = qu->prev;
  }

  /* create node with new key */
  new = ( tmp != NULL ) ? tmp : get_node();
  new->object = new_obj;
  new->key = new_key;
  /* insert node in rear of minkey queue, as qu->prev->next */
  new->prev = qu->prev;
  qu->prev->next->prev = new;
  new->next = qu->prev->next;
  qu->prev->next = new;
}

object_t*
dequeue(queue_t* qu)
{
  queue_t* tmp;
  object_t* tmp_object;

  if ( qu->next->next == qu->next)
  { return ( NULL ); } /* dequeue from empty queue */

  else {
    /* unlink node from front of object queue */
    tmp = qu->next->prev;
    tmp_object = tmp->object;
    qu->next->prev = tmp->prev;
    qu->next->prev->next = qu->next;

    /* test front of minqueue, unlink node if equal */
    if ( tmp->key == qu->prev->prev->key ) {
      return_node( tmp );
      tmp = qu->prev->prev;
      qu->prev->prev = tmp->prev;
      qu->prev->prev->next = qu->prev;
    }

    return_node( tmp );
    return ( tmp_object );
  }
}

void
remove_minqueue(queue_t* qu)
{
  queue_t* tmp;
  /* link allqueues together to a list connected by next */
  qu->next->prev->next = qu->prev;
  qu->prev->prev->next = NULL;
  /* follow the next pointers and return all nodes*/

  do {
    tmp = qu->next;
    return_node( qu );
    qu = tmp;

  } while ( qu != NULL );
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  queue_t* m_qu;
  char nextop;
  m_qu = create_minqueue();
  printf("Made Minqueue\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'e' ) {
      int inskey, *insobj, success;
      insobj = (object_t*) malloc(sizeof(object_t));
      scanf(" %d", &inskey);
      *insobj = 10 * inskey + 1;
      enqueue( insobj, inskey, m_qu );
      printf(" enqueued %d. The current minimum key is %d\n", inskey, find_min_key(m_qu) );
    }

    if ( nextop == 'd' ) {
      object_t* delobj;
      getchar();
      delobj = dequeue(m_qu);

      if ( delobj == NULL )
      { printf("  dequeue failed\n"); }

      else
      { printf("  dequeued object %d", *delobj); }

      if ( queue_empty(m_qu) )
      { printf(" the min_queue is now empty\n"); }

      else
      { printf(" the minimum key is now %d\n",  find_min_key(m_qu) ); }

    }

    if ( nextop == '?' ) {
      getchar();

      if ( queue_empty(m_qu) )
      { printf("the min_queue is empty\n"); }

      else
      { printf("  the minimum key is %d\n",  find_min_key(m_qu) ); }
    }

  }
  remove_minqueue(m_qu);
  printf(" removed minqueue\n");

  return (0);
}
