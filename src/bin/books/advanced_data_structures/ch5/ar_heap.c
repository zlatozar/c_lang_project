#include <stdlib.h>
#include <stdio.h>

typedef int key_t;

typedef int object_t;

typedef struct {key_t key; object_t* object;} heap_el_t;

typedef struct {
  int     max_size;
  int current_size;
  heap_el_t*  heap;
} heap_t;

// _____________________________________________________________________________

heap_t*
create_heap(int size)
{
  heap_t* hp;

  hp = (heap_t*) malloc( sizeof(heap_t) );
  hp->heap = (heap_el_t*) malloc( size * sizeof(heap_el_t) );
  hp->max_size = size;
  hp->current_size = 0;

  return ( hp );
}

int
heap_empty(heap_t* hp)
{
  return ( hp->current_size == 0 );
}

heap_el_t*
find_min(heap_t* hp)
{
  return ( hp->heap );
}

int
insert(key_t new_key, object_t* new_object, heap_t* hp)
{
  if ( hp->current_size < hp->max_size ) {
    int gap;
    gap = hp->current_size++;

    while (gap > 0 && new_key < (hp->heap[(gap - 1) / 2]).key ) {
      (hp->heap[gap]).key    = (hp->heap[(gap - 1) / 2]).key;
      (hp->heap[gap]).object = (hp->heap[(gap - 1) / 2]).object;
      gap = (gap - 1) / 2;
    }

    (hp->heap[gap]).key    = new_key;
    (hp->heap[gap]).object = new_object;
    return ( 0 );

  } else
  { return ( -1 ); } /* Heap overflow */
}

object_t*
delete_min(heap_t* hp)
{
  object_t* del_obj;
  int reached_top = 0;
  int gap, newgap, last;

  if ( hp->current_size == 0 )
  { return ( NULL ); } /*delete from empty heap, failed */

  del_obj = (hp->heap[0]).object;
  gap = 0;

  while ( ! reached_top ) {

    if ( 2 * gap + 2 < hp->current_size ) {

      if ( (hp->heap[2 * gap + 1]).key < (hp->heap[2 * gap + 2]).key)
      { newgap = 2 * gap + 1; }

      else
      { newgap = 2 * gap + 2; }

      (hp->heap[gap]).key    = (hp->heap[newgap]).key;
      (hp->heap[gap]).object = (hp->heap[newgap]).object;
      gap = newgap;

    } else if ( 2 * gap + 2 == hp->current_size ) {
      newgap = 2 * gap + 1;
      (hp->heap[gap]).key    = (hp->heap[newgap]).key;
      (hp->heap[gap]).object = (hp->heap[newgap]).object;
      hp->current_size -= 1;

      return (del_obj);
      /* finished, came out exactly on last element */
    } else
    { reached_top = 1; }
  }

  /* propagated gap to the top, now move gap down again */
  /* to insert last object in the right place */
  last = --hp->current_size;
  while (gap > 0 && (hp->heap[last]).key < (hp->heap[(gap - 1) / 2]).key ) {
    (hp->heap[gap]).key    = (hp->heap[(gap - 1) / 2]).key;
    (hp->heap[gap]).object = (hp->heap[(gap - 1) / 2]).object;
    gap = (gap - 1) / 2;
  }

  (hp->heap[gap]).key    = (hp->heap[last]).key;
  (hp->heap[gap]).object = (hp->heap[last]).object;
  /* filled gap by moving last element in it*/
  return ( del_obj );
}

void
remove_heap(heap_t* hp)
{
  free( hp->heap );
  free( hp );
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  heap_t* heap;
  char nextop;
  heap = create_heap(1000);
  printf("Made Heap\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int inskey,  *insobj, success;
      insobj = (object_t*) malloc(sizeof(object_t));
      scanf(" %d,%d", &inskey, insobj);
      success = insert( inskey, insobj, heap );

      if ( success == 0 )
        printf("  insert successful, key = %d, object value = %d, \
current heap size is %d\n", inskey, *insobj, heap->current_size );

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
        printf("  delete successful, deleted object %d\n",
               *delobj);
    }
  }
  return (0);
}
