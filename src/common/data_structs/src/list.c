#include "data_structs/list.h"

#include "lang/assert.h"
#include "lang/mem.h"

struct list {
  void* head;
  struct list* tail;
};

//______________________________________________________________________________
//                                                                       Public

List_T
List_cons(void* head, List_T this)
{
  List_T listT;
  NEW(listT);

  listT->head = head;
  listT->tail = this;

  return listT;
}

void
List_free(List_T* p_this)
{
  List_T restT;

  Assert(p_this && *p_this);

  for ( ; *p_this; *p_this = restT) {
    restT = (*p_this)->tail;
    FREE(*p_this);
  }
}

void*
List_head(List_T this)
{
  /* Fails if out of bound */
  Assert(this);
  return this->head;
}

/*
 * NOTE: Commonly used in iterations to take next:
 *       this = List_tail(this);
 */
List_T
List_tail(List_T this)
{
  Assert(this);
  return this->tail;
}

size_t
List_length(List_T this)
{
  size_t accu = 0;

  while (this != NULL) {
    ++accu;
    this = List_tail(this);
  }

  return accu;
}

void
List_drop(List_T* p_this, unsigned n)
{
  List_T restT;

  Assert(p_this && *p_this);

  for (; *p_this && n != 0; *p_this = restT) {
    restT = (*p_this)->tail;
    FREE(*p_this);
    --n;
  }
}

void*
List_nth(List_T this, unsigned n)
{
  while (n != 0) {
    this = List_tail(this);
    n--;
  }

  return List_head(this);
}

List_T
List_append(List_T xlistT, List_T ylistT)
{
  List_T resultT = ylistT;
  List_T* p_resultT = &resultT;

  while (xlistT != NULL) {
    List_T newT = List_cons(List_head(xlistT), ylistT);

    *p_resultT = newT;
    p_resultT = &newT->tail;

    xlistT = List_tail(xlistT);
  }

  return resultT;
}

List_T
List_filter(bool (*pred_fn)(const void*), List_T this)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while (this != NULL) {
    void* curr = List_head(this);

    if (pred_fn(curr)) {
      List_T newT = List_cons(curr, NULL);

      *p_resultT = newT;
      p_resultT = &newT->tail;
    }

    this = List_tail(this);
  }

  return resultT;
}

// for two argument predicate: e.g. if (arg > curr) {..
List_T
List_filter2(bool (*pred_fn)(void*, const void*), void* arg, List_T this)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while (this != NULL) {
    void* curr = List_head(this);

    if (pred_fn(arg, curr)) {
      List_T newT = List_cons(curr, NULL);

      *p_resultT = newT;
      p_resultT = &newT->tail;
    }

    this = List_tail(this);
  }

  return resultT;
}

List_T
List_map(void* (*map_fn)(void*), List_T this)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while (this != NULL) {
    List_T newT = List_cons(map_fn(List_head(this)), NULL);

    *p_resultT = newT;
    p_resultT = &newT->tail;

    this = List_tail(this);
  }

  return resultT;
}

List_T
List_map2(void* (*map_fn)(void*, void*), void* arg, List_T this)
{
  List_T resultT = NULL;
  List_T* p_resultT = &resultT;

  while (this != NULL) {
    List_T newT = List_cons(map_fn(arg, List_head(this)), NULL);

    *p_resultT = newT;
    p_resultT = &newT->tail;

    this = List_tail(this);
  }

  return resultT;
}

List_T
List_copy(List_T this)
{
  List_T newT = NULL;
  List_T* p_newT = &newT;

  for ( ; this; this = this->tail) {
    NEW(*p_newT);
    (*p_newT)->head = this->head;

    p_newT = &(*p_newT)->tail;
  }

  *p_newT = NULL;
  return newT;
}

void
List_print(void (*print_fn)(const void*), List_T this)
{
  while (this != NULL) {
    print_fn(List_head(this));

    this = List_tail(this);
  }
}
