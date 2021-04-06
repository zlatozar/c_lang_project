#include "data_structs/double_list.h"

#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

/* __________________________________________________________________________ */
/*                                                                     Local  */

struct double_node {
  Generic_T datapointer;
  struct double_node* next;
  struct double_node* prev;
};

#define DATA(p_node) ((p_node)->datapointer)
#define NEXT(p_node) ((p_node)->next)
#define PREV(p_node) ((p_node)->prev)

/* `pp_node` points position where node should be situated. */
static void
__allocate_double_node(doublenode_t** pp_node, Generic_T data)
{
  doublenode_t* p_node;
  NEW(p_node);

  *pp_node = p_node;

  DATA(p_node) = data;
  NEXT(p_node) = NULL;
  PREV(p_node) = NULL;
}

static void
__free_double_node(doublenode_t** pp_node)
{
  FREE(*pp_node);
}

/* Delete given `p_node` from `p_list`. */
static void
__delete_double_node(DoubleList_T* p_list, doublenode_t* p_node)
{
  DoubleList_T prev, next;

  prev = PREV(p_node);
  next = NEXT(p_node);

  if (prev != NULL)
  { NEXT(prev) = next; }

  if (next != NULL)
  { PREV(next) = prev; }

  if (p_node == *p_list) {
    if (next != NULL) {
      *p_list = next;

    } else {
      *p_list = prev;
    }
  }

  __free_double_node(&p_node);
}

/* __________________________________________________________________________ */

DoubleList_T
DoubleList_new(void)
{
  DoubleList_T list = NULL;
  return list;
}

bool
DoubleList_is_empty(DoubleList_T list)
{
  return list == NULL;
}

void
DoubleList_insert(DoubleList_T* p_list, Generic_T data)
{
  Require(p_list);

  doublenode_t* new_node;
  __allocate_double_node(&new_node, data);

  if (DoubleList_is_empty(*p_list)) {
    PREV(new_node) = NULL;
    NEXT(new_node) = NULL;

  } else {
    NEXT(new_node) = *p_list;
    PREV(new_node) = PREV(*p_list);

    /* Is at the beginning or "somewhere" inside list. */
    PREV(*p_list) = new_node;
    if (PREV(new_node) != NULL)
    { NEXT(PREV(new_node)) = new_node; }
  }

  *p_list = new_node;
}

void
DoubleList_append(DoubleList_T* p_list, Generic_T data)
{
  doublenode_t* p_node;
  __allocate_double_node(&p_node, data);

  if (DoubleList_is_empty(*p_list)) {
    *p_list = p_node;

  } else {
    doublenode_t* last_node;
    for (last_node = *p_list; NEXT(last_node) != NULL; ) {
      last_node = NEXT(last_node);
    }

    NEXT(last_node) = p_node;
    PREV(p_node) = last_node;
  }
}

size_t
DoubleList_length(DoubleList_T list)
{
  size_t n;
  for (n = 0; list != NULL; list = NEXT(list))
  { n++; }

  return n;
}

doublenode_t*
DoubleList_nth(DoubleList_T list, int idx)
{
  Require(idx != 0);

  if (DoubleList_is_empty(list)) {
    return NULL;
  }

  int iter = 1;
  doublenode_t* nth = list;

  if (idx > 0) {
    while (iter < idx && nth != NULL) {
      nth = NEXT(nth);
      ++iter;
    }
    return nth;
  }

  /* For negative index we start from the end. */
  while (NEXT(nth) != NULL) {
    nth = NEXT(nth);
  }

  iter = -1;
  while (iter > idx && nth != NULL) {
    nth = PREV(nth);
    --iter;
  }

  return nth;
}

bool
DoubleList_delete_head(DoubleList_T* p_list, Generic_T* p_data__)
{
  if (DoubleList_is_empty(*p_list)) {
    Log_debug("Can't delete from empty double list.");
    return false;
  }

  *p_data__ = DATA(*p_list);
  __delete_double_node(p_list, *p_list /* Fist node in practice. */);

  return true;
}

bool
DoubleList_traverse(DoubleList_T list, bool (*apply_fn)(Generic_T))
{
  Require(apply_fn);

  if (DoubleList_is_empty(list))
  { return true; }

  if (!(*apply_fn)(DATA(list))) {
    Log_error("Can't apply function.");
    return false;

  } else {
    return DoubleList_traverse(NEXT(list), apply_fn);
  }
}

/* `p_start` could be used as beginning of double linked list. */
void
DoubleList_cut(DoubleList_T* p_list, doublenode_t** p_start, doublenode_t** p_end)
{
  Require(p_list);

  DoubleList_T start, end;

  /* Keep the "original" pointers. */
  start = *p_start;
  end = *p_end;

  if (PREV(start))
  { NEXT(PREV(start)) = NEXT(end); }

  if (NEXT(end))
  { PREV(NEXT(end)) = PREV(start); }

  if (*p_list == start)
  { *p_list = NEXT(end); }

  PREV(start) = NEXT(end) = NULL;
}

void
DoubleList_paste(DoubleList_T* p_target, DoubleList_T* p_source)
{
  DoubleList_T target, source, lastnode;

  if (DoubleList_is_empty(*p_source))
  { return; }

  if (DoubleList_is_empty(*p_target)) {
    *p_target = *p_source;

  } else {
    source = *p_source;
    target = *p_target;

    lastnode = DoubleList_nth(source, -1);

    NEXT(lastnode) = NEXT(target);
    if (NEXT(target) != NULL)
    { PREV(NEXT(target)) = lastnode; }

    PREV(source) = target;
    NEXT(target) = source;
  }

  *p_source = NULL;
}

void
DoubleList_destroy(DoubleList_T* p_list, free_data_FN free_data_fn)
{
  Require(p_list);

  /* First iteration will initialize it. */
  DoubleList_T next_tmp;
  for ( ; *p_list != NULL; *p_list = next_tmp) {
    next_tmp = NEXT(*p_list);

    if (free_data_fn != NULL) {
      free_data_fn(DATA(*p_list));
    }

    FREE(*p_list);
  }
}

void
DoubleList_free(DoubleList_T* p_list)
{
  DoubleList_destroy(p_list, NULL);
}
