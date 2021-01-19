#include <limits.h>  /* INT_MAX      */
#include <stddef.h>  /* NULL, size_t */

#include "macros/lang.h"
#include "common/lang/assert.h"
#include "common/lang/mem.h"
#include "common/math/arith.h"

#include "common/data_structs/set.h"

struct set {
  size_t length;
  unsigned time_stamp;
  int (*cmp)( const void* x, const void* y );
  size_t (*hash)( const void* x );
  size_t size;

  struct member {
    struct member *link;
    const void* member;
  } **buckets;
};

LOCAL int cmpatom(const void* x, const void* y)
{
  return x != y;
}

LOCAL size_t hashatom(const void* x)
{
  return (unsigned long)x >> 2;
}

LOCAL Set_T copy(Set_T t, size_t hint)
{
  Set_T set;
  Assert(t);

  set = Set_new(hint, t->cmp, t->hash);
  {
    size_t i;

    struct member *q;
    for (i = 0; i < t->size; i++) {
      for (q = t->buckets[i]; q; q = q->link) {
        struct member *p;
        const void* member = q->member;

        size_t ii = (*set->hash)(member) % set->size;

        NEW(p);

        p->member = member;
        p->link = set->buckets[ii];

        set->buckets[ii] = p;
        set->length++;
      }
    }
  }

  return set;
}

Set_T Set_new(size_t hint, int cmp( const void* x, const void* y ),
              size_t hash( const void* x ))
{
  Set_T set;
  size_t i;

  static unsigned primes[] = { 509, 509, 1021, 2053, 4093,
                               8191, 16381, 32771, 65521, INT_MAX };

  for (i = 1; primes[i] < hint; i++)
    ;

  set = ALLOC(sizeof(*set) + primes[i-1] * sizeof(set->buckets[0]));

  set->size = primes[i-1];
  set->cmp  = cmp  ?  cmp : cmpatom;
  set->hash = hash ? hash : hashatom;
  set->buckets = (struct member **)(set + 1);

  for (i = 0; i < set->size; i++)
    set->buckets[i] = NULL;

  set->length = 0;
  set->time_stamp = 0;

  return set;
}

void Set_free(Set_T* set)
{
  Assert(set && *set);

  if ((*set)->length > 0) {
    size_t i;
    struct member *p, *q;

    for (i = 0; i < (*set)->size; i++) {
      for (p = (*set)->buckets[i]; p; p = q) {
        q = p->link;

        FREE(p);
      }
    }
  }

  FREE(*set);
}

size_t Set_length(Set_T set)
{
  Assert(set);
  return set->length;
}

int Set_member(Set_T set, const void* member)
{
  size_t i;
  struct member *p;

  Assert(set);
  Assert(member);

  i = (*set->hash)(member) % set->size;

  for (p = set->buckets[i]; p; p = p->link) {
    if ((*set->cmp)(member, p->member) == 0)
      break;
  }

  return p != NULL;
}

void Set_put(Set_T set, const void* member)
{
  size_t i;
  struct member *p;

  Assert(set);
  Assert(member);

  i = (*set->hash)(member)%set->size;
  for (p = set->buckets[i]; p; p = p->link) {
    if ((*set->cmp)(member, p->member) == 0)
      break;
  }

  if (p == NULL) {
    NEW(p);

    p->member = member;
    p->link = set->buckets[i];

    set->buckets[i] = p;
    set->length++;

  } else {
    p->member = member;
  }

  set->time_stamp++;
}

void* Set_remove(Set_T set, const void* member)
{
  size_t i;
  struct member **pp;

  Assert(set);
  Assert(member);

  set->time_stamp++;
  i = (*set->hash)(member) % set->size;

  for (pp = &set->buckets[i]; *pp; pp = &(*pp)->link) {
    if ((*set->cmp)(member, (*pp)->member) == 0) {
      struct member *p = *pp;
      *pp = p->link;
      member = p->member;

      FREE(p);

      set->length--;

      return (void *)member;
    }
  }

  return NULL;
}

void Set_map(Set_T set, void apply( const void* member, void* cl ), void* cl)
{
  size_t i;
  unsigned stamp;
  struct member *p;

  Assert(set);
  Assert(apply);

  stamp = set->time_stamp;
  for (i = 0; i < set->size; i++) {

    for (p = set->buckets[i]; p; p = p->link) {
      apply(p->member, cl);
      Assert(set->time_stamp == stamp);
    }
  }
}

void** Set_toArray(Set_T set, void* end)
{
  size_t i, j = 0;
  void** array;
  struct member *p;

  Assert(set);

  array = ALLOC((set->length + 1) * sizeof(*array));

  for (i = 0; i < set->size; i++) {
    for (p = set->buckets[i]; p; p = p->link)
      array[j++] = (void *)p->member;
  }

  array[j] = end;
  return array;
}

Set_T Set_union(Set_T s, Set_T t)
{
  if (s == NULL) {
    Assert(t);
    return copy(t, t->size);

  } else if (t == NULL) {
    return copy(s, s->size);

  } else {
    Set_T set = copy(s, Arith_smax(s->size, t->size));

    Assert(s->cmp == t->cmp && s->hash == t->hash);

    {
      size_t i;
      struct member *q;
      for (i = 0; i < t->size; i++) {
        for (q = t->buckets[i]; q; q = q->link)
          Set_put(set, q->member);
      }
    }

    return set;
  }
}

Set_T Set_inter(Set_T s, Set_T t)
{
  if (s == NULL) {
    Assert(t);
    return Set_new(t->size, t->cmp, t->hash);

  } else if (t == NULL) {
    return Set_new(s->size, s->cmp, s->hash);

  } else if (s->length < t->length) {
    return Set_inter(t, s);

  } else {
    Set_T set = Set_new(Arith_smin(s->size, t->size), s->cmp, s->hash);

    Assert(s->cmp == t->cmp && s->hash == t->hash);

    {
      size_t i;

      struct member *q;
      for (i = 0; i < t->size; i++) {

        for (q = t->buckets[i]; q; q = q->link)
          if (Set_member(s, q->member)) {
            struct member *p;
            const void* member = q->member;

            size_t ii = (*set->hash)(member) % set->size;

            NEW(p);

            p->member = member;
            p->link = set->buckets[ii];

            set->buckets[ii] = p;
            set->length++;
          }
      }
    }

    return set;
  }
}

Set_T Set_minus(Set_T t, Set_T s)
{
  if (t == NULL) {
    Assert(s);
    return Set_new(s->size, s->cmp, s->hash);

  } else if (s == NULL) {
    return copy(t, t->size);

  } else {
    Set_T set = Set_new(Arith_smin(s->size, t->size), s->cmp, s->hash);

    Assert(s->cmp == t->cmp && s->hash == t->hash);

    {
      size_t i;

      struct member *q;
      for (i = 0; i < t->size; i++)
        for (q = t->buckets[i]; q; q = q->link)

          if (!Set_member(s, q->member)) {
            struct member *p;
            const void* member = q->member;

            size_t ii = (*set->hash)(member) % set->size;

            NEW(p);

            p->member = member;
            p->link = set->buckets[ii];

            set->buckets[ii] = p;
            set->length++;
          }
    }

    return set;
  }
}

Set_T Set_diff(Set_T s, Set_T t)
{
  if (s == NULL) {
    Assert(t);
    return copy(t, t->size);

  } else if (t == NULL) {
    return copy(s, s->size);

  } else {
    Set_T set = Set_new(Arith_smin(s->size, t->size), s->cmp, s->hash);
    Assert(s->cmp == t->cmp && s->hash == t->hash);

    {
      size_t i;

      struct member *q;
      for (i = 0; i < t->size; i++)
        for (q = t->buckets[i]; q; q = q->link)

          if (!Set_member(s, q->member)) {
            struct member *p;
            const void* member = q->member;

            size_t ii = (*set->hash)(member) % set->size;

            NEW(p);

            p->member = member;
            p->link = set->buckets[ii];

            set->buckets[ii] = p;
            set->length++;
          }
    }

    { Set_T u = t; t = s; s = u; }

    {
      size_t i;

      struct member *q;
      for (i = 0; i < t->size; i++)
        for (q = t->buckets[i]; q; q = q->link)

          if (!Set_member(s, q->member)) {
            struct member *p;
            const void* member = q->member;

            size_t ii = (*set->hash)(member) % set->size;

            NEW(p);

            p->member = member;
            p->link = set->buckets[ii];

            set->buckets[ii] = p;
            set->length++;
        }
    }

    return set;
  }
}
