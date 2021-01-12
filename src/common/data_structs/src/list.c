#include <stddef.h>  /* NULL, size_t */

#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/list.h"

struct List_T {
  void* head;
  struct List_T* tail;
};

List_T List_cons(void* head, List_T tail)
{
  List_T list;
  NEW(list);

  list->head = head;
  list->tail = tail;

  return list;
}

void List_free(List_T* list)
{
  List_T rest;

  Assert(list);

  for( ; *list; *list = rest) {
    rest = (*list)->tail;
    FREE(*list);
  }
}

void* List_head(List_T list)
{
  /* Fails if out of bound */
  Assert(list);
  return list->head;
}

List_T List_tail(List_T list)
{
  Assert(list);
  return list->tail;
}

size_t List_length(List_T list)
{
  size_t accu = 0;

  while(list != NULL) {
    ++accu;
    list = List_tail(list);
  }

  return accu;
}

// FIXME
void List_drop(List_T list, unsigned n)
{
  Assert(list);

  while (n != 0) {
    List_T current = list;

    list = List_tail(list);
    FREE(current);
    n--;
  }
}

void* List_nth(List_T list, unsigned n)
{
  while (n != 0) {
    list = List_tail(list);
    n--;
  }

  return List_head(list);
}

List_T List_append(List_T xs, List_T ys)
{
  List_T accu = ys;
  List_T* last = &accu;

  while(xs != NULL) {
    List_T new = List_cons( List_head(xs), ys);
    *last = new;

    last = &new->tail;
    xs = List_tail(xs);
  }

  return accu;
}

List_T List_filter(bool (*pred)( const void* ), List_T xs)
{
  Assert(xs);

  List_T accu = NULL;
  List_T *last = &accu;

  while(xs != NULL) {
    void* x = List_head(xs);

    if(pred( x )) {
      List_T new = List_cons(x, NULL);
      *last = new;
      last = &new->tail;
    }
    xs = List_tail(xs);
  }

  return accu;
}

// for two argument predicate: e.g. arg > current element
List_T List_filter2(bool (*pred)( void*, const void* ), void* arg, List_T xs)
{
  Assert(xs);

  List_T accu = NULL ;
  List_T *last = &accu;

  while(xs != NULL) {
    void* x = List_head(xs);

    if(pred( arg, x )) {
      List_T new = List_cons(x, NULL);
      *last = new;
      last = &new->tail;
    }
    xs = List_tail(xs);
  }

  return accu;
}

List_T List_map(void* (*f)( void* ), List_T xs)
{
  Assert(xs);

  List_T accu = NULL;
  List_T *last = &accu;

  while(xs != NULL) {
    List_T new = List_cons( f( List_head(xs) ), NULL);
    *last = new;

    last = &new->tail;
    xs = List_tail(xs);
  }
  return accu;
}

List_T List_map2(void* (*f)( void*, void* ), void* arg, List_T xs)
{
  Assert(xs);

  List_T accu = NULL;
  List_T *last = &accu;

  while(xs != NULL) {
    List_T new = List_cons( f( arg, List_head(xs) ), NULL);
    *last = new;

    last = &new->tail;
    xs = List_tail(xs);
  }

  return accu;
}

List_T List_copy(List_T xs)
{
  List_T accu = NULL;
  List_T *last = &accu;

  Assert(xs);

  while(xs != NULL) {
    List_T new = List_cons( List_head(xs), NULL);
    *last = new;

    last = &new->tail;
    xs = List_tail(xs);
  }

  return accu;
}

void List_print(void (*print)( const void* ), List_T xs)
{
  while (xs != NULL) {
    print( List_head(xs));
    xs = List_tail(xs);
  }
}
