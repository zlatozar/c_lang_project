#include <stddef.h>  /* NULL, size_t */
#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/list.h"

struct list {
  void* head;
  struct list *tail;
};

List_T List_cons(void* head, List_T self)
{
  List_T listT;
  NEW(listT);

  listT->head = head;
  listT->tail = self;

  return listT;
}

void List_free(List_T* p_self)
{
  List_T restT;

  Assert(p_self && *p_self);

  for( ; *p_self; *p_self = restT) {
    restT = (*p_self)->tail;
    FREE(*p_self);
  }
}

void* List_head(List_T self)
{
  /* Fails if out of bound */
  Assert(self);
  return self->head;
}

/*
 * NOTE: Commonly used in iterations to take next:
 *       self = List_tail(self);
 */
List_T List_tail(List_T self)
{
  Assert(self);
  return self->tail;
}

size_t List_length(List_T self)
{
  size_t accu = 0;

  while(self != NULL) {
    ++accu;
    self = List_tail(self);
  }
  return accu;
}

void List_drop(List_T* p_self, unsigned n)
{
  List_T restT;

  Assert(p_self && *p_self);

  for( ; *p_self && n != 0; *p_self = restT) {
    restT = (*p_self)->tail;
    FREE(*p_self);
    --n;
  }
}

void* List_nth(List_T self, unsigned n)
{
  while (n != 0) {
    self = List_tail(self);
    n--;
  }
  return List_head(self);
}

List_T List_append(List_T xlistT, List_T ylistT)
{
  List_T resultT = ylistT;
  List_T* p_resultT = &resultT;

  while(xlistT != NULL) {
    List_T newT = List_cons(List_head(xlistT), ylistT);

    *p_resultT = newT;
    p_resultT = &newT->tail;

    xlistT = List_tail(xlistT);
  }
  return resultT;
}

List_T List_filter(bool (*pred_fn)( const void* ), List_T self)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while(self != NULL) {
    void* curr = List_head(self);

    if(pred_fn(curr)) {
      List_T newT = List_cons(curr, NULL);

      *p_resultT = newT;
      p_resultT = &newT->tail;
    }

    self = List_tail(self);
  }
  return resultT;
}

// for two argument predicate: e.g. if (arg > curr) {..
List_T List_filter2(bool (*pred_fn)( void*, const void* ), void* arg, List_T self)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while(self != NULL) {
    void* curr = List_head(self);

    if(pred_fn(arg, curr)) {
      List_T newT = List_cons(curr, NULL);

      *p_resultT = newT;
      p_resultT = &newT->tail;
    }

    self = List_tail(self);
  }
  return resultT;
}

List_T List_map(void* (*map_fn)( void* ), List_T self)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while(self != NULL) {
    List_T newT = List_cons(map_fn(List_head(self)), NULL);

    *p_resultT = newT;
    p_resultT = &newT->tail;

    self = List_tail(self);
  }
  return resultT;
}

List_T List_map2(void* (*map_fn)( void*, void* ), void* arg, List_T self)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while(self != NULL) {
    List_T newT = List_cons(map_fn(arg, List_head(self)), NULL);

    *p_resultT = newT;
    p_resultT = &newT->tail;

    self = List_tail(self);
  }
  return resultT;
}

List_T List_copy(List_T self) {
  List_T newT = NULL;
  List_T *p_newT = &newT;

  for ( ; self; self = self->tail) {
    NEW(*p_newT);
    (*p_newT)->head = self->head;

    p_newT = &(*p_newT)->tail;
  }

  *p_newT = NULL;
  return newT;
}

void List_print(void (*print_fn)( const void* ), List_T self)
{
  while (self != NULL) {
    print_fn(List_head(self));

    self = List_tail(self);
  }
}
