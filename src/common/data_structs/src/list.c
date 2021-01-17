#include <stddef.h>  /* NULL, size_t */

#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/list.h"

struct _List_T {
  void* head;
  struct _List_T* tail;
};

List_T List_cons(void* head, List_T listT)
{
  List_T newT;
  NEW(newT);

  newT->head = head;
  newT->tail = listT;

  return newT;
}

void List_free(List_T* p_listT)
{
  List_T restT;

  Assert(p_listT && *p_listT);

  for( ; *p_listT; *p_listT = restT) {
    restT = (*p_listT)->tail;
    FREE(*p_listT);
  }
}

void* List_head(List_T listT)
{
  /* Fails if out of bound */
  Assert(listT);
  return listT->head;
}

/*
 * NOTE: Commonly used in iterations to take next:
 *       listT = List_tail(listT);
 */
List_T List_tail(List_T listT)
{
  Assert(listT);
  return listT->tail;
}

size_t List_length(List_T listT)
{
  size_t accu = 0;

  while(listT != NULL) {
    ++accu;
    listT = List_tail(listT);
  }
  return accu;
}

void List_drop(List_T* p_listT, unsigned n)
{
  List_T restT;

  Assert(p_listT && *p_listT);

  for( ; *p_listT && n != 0; *p_listT = restT) {
    restT = (*p_listT)->tail;
    FREE(*p_listT);
    --n;
  }
}

void* List_nth(List_T listT, unsigned n)
{
  while (n != 0) {
    listT = List_tail(listT);
    n--;
  }
  return List_head(listT);
}

List_T List_append(List_T x_listT, List_T y_listT)
{
  List_T resultT = y_listT;
  List_T* p_y_listT = &resultT;

  while(x_listT != NULL) {
    List_T new_ref = List_cons(List_head(x_listT), y_listT);

    *p_y_listT = new_ref;
    p_y_listT = &new_ref->tail;

    x_listT = List_tail(x_listT);
  }
  return resultT;
}

List_T List_filter(bool (*pred_fn)( const void* ), List_T listT)
{
  List_T resultT = NULL;
  List_T* p_restT = &resultT;

  while(listT != NULL) {
    void* curr = List_head(listT);

    if(pred_fn(curr)) {
      List_T new_ref = List_cons(curr, NULL);

      *p_restT = new_ref;
      p_restT = &new_ref->tail;
    }

    listT = List_tail(listT);
  }
  return resultT;
}

// for two argument predicate: e.g. if (arg > curr) {..
List_T List_filter2(bool (*pred_fn)( void*, const void* ), void* arg, List_T listT)
{
  List_T resultT = NULL;
  List_T* p_restT = &resultT;

  while(listT != NULL) {
    void* curr = List_head(listT);

    if(pred_fn(arg, curr)) {
      List_T new_ref = List_cons(curr, NULL);

      *p_restT = new_ref;
      p_restT = &new_ref->tail;
    }

    listT = List_tail(listT);
  }
  return resultT;
}

List_T List_map(void* (*map_fn)( void* ), List_T listT)
{
  List_T resultT = NULL;
  List_T* p_restT = &resultT;

  while(listT != NULL) {
    List_T new_ref = List_cons( map_fn(List_head(listT) ), NULL);

    *p_restT = new_ref;
    p_restT = &new_ref->tail;

    listT = List_tail(listT);
  }
  return resultT;
}

List_T List_map2(void* (*map_fn)( void*, void* ), void* arg, List_T listT)
{
  List_T resultT = NULL;
  List_T* p_restT = &resultT;

  while(listT != NULL) {
    List_T new_ref = List_cons( map_fn(arg, List_head(listT) ), NULL);

    *p_restT = new_ref;
    p_restT = &new_ref->tail;

    listT = List_tail(listT);
  }
  return resultT;
}

List_T List_copy(List_T listT) {
  List_T newT, *p_newT = &newT;

  for ( ; listT; listT = listT->tail) {
    NEW(*p_newT);
    (*p_newT)->head = listT->head;

    p_newT = &(*p_newT)->tail;
  }

  *p_newT = NULL;
  return newT;
}

void List_print(void (*print_fn)( const void* ), List_T listT)
{
  while (listT != NULL) {
    print_fn(List_head(listT));

    listT = List_tail(listT);
  }
}
