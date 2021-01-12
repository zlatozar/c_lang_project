#include <limits.h>  /* INT_MAX      */
#include <stddef.h>  /* NULL, size_t */

#include "macros/lang.h"
#include "common/lang/assert.h"
#include "common/lang/mem.h"

#include "common/data_structs/table.h"

struct Table_T {
  size_t size;
  int (*cmp)( const void* x, const void* y);
  unsigned (*hash)( const void* key );
  size_t length;
  unsigned timestamp;

  struct binding {
    struct binding *link;
    const void* key;
    void* value;
  } **buckets;
};

LOCAL int cmpatom(const void* x, const void* y)
{
  return x != y;
}

LOCAL unsigned hashatom(const void* key)
{
  return (unsigned long)key >> 2;
}

Table_T Table_new(unsigned hint, int cmp( const void* x, const void* y),
                  unsigned hash( const void* key ))
{
  Table_T table;
  size_t i;

  static unsigned primes[] = { 509, 509, 1021, 2053, 4093,
                               8191, 16381, 32771, 65521, INT_MAX };

  for (i = 1; primes[i] < hint; i++)
    ;

  table = ALLOC(sizeof (*table) + primes[i - 1] * sizeof (table->buckets[0]));

  table->size = primes[i - 1];
  table->cmp  = cmp  ?  cmp : cmpatom;
  table->hash = hash ? hash : hashatom;
  table->buckets = (struct binding **)(table + 1);

  for (i = 0; i < table->size; i++)
    table->buckets[i] = NULL;

  table->length = 0;
  table->timestamp = 0;

  return table;
}

void Table_free(Table_T* table)
{

  Assert(table && *table);

  if ((*table)->length > 0) {
    size_t i;
    struct binding *p, *q;

    for (i = 0; i < (*table)->size; i++)
      for (p = (*table)->buckets[i]; p; p = q) {
        q = p->link;

        FREE(p);
      }
  }

  FREE(*table);
}

size_t Table_length(Table_T table)
{
  Assert(table);
  return table->length;
}

void* Table_put(Table_T table, const void* key, void* value)
{
  int i;
  struct binding *p;
  void* prev;

  Assert(table);
  Assert(key);

  i = (*table->hash)(key) % table->size;

  for (p = table->buckets[i]; p; p = p->link)
    if ((*table->cmp)(key, p->key) == 0)
      break;

  if (p == NULL) {
    NEW(p);

    p->key = key;
    p->link = table->buckets[i];

    table->buckets[i] = p;
    table->length++;

    prev = NULL;

  } else {
    prev = p->value;
  }

  p->value = value;
  table->timestamp++;

  return prev;
}

void* Table_get(Table_T table, const void* key)
{
  int i;
  struct binding *p;

  Assert(table);
  Assert(key);

  i = (*table->hash)(key) % table->size;

  for (p = table->buckets[i]; p; p = p->link)
    if ((*table->cmp)(key, p->key) == 0)
      break;

  return p ? p->value : NULL;
}

void* Table_remove(Table_T table, const void* key)
{
  int i;
  struct binding **pp;

  Assert(table);
  Assert(key);

  table->timestamp++;
  i = (*table->hash)(key) % table->size;

  for (pp = &table->buckets[i]; *pp; pp = &(*pp)->link)

    if ((*table->cmp)(key, (*pp)->key) == 0) {
      struct binding *p = *pp;
      void* value = p->value;
      *pp = p->link;

      FREE(p);

      table->length--;

      return value;
    }

  return NULL;
}

void Table_map(Table_T table, void apply( const void* key, void** value, void* cl ),
               void* cl)
{
  size_t i;
  unsigned stamp;
  struct binding *p;

  Assert(table);
  Assert(apply);

  stamp = table->timestamp;

  for (i = 0; i < table->size; i++)

    for (p = table->buckets[i]; p; p = p->link) {
      apply(p->key, &p->value, cl);
      Assert(table->timestamp == stamp);
    }
}

void** Table_toArray(Table_T table, void* end)
{
  size_t i, j = 0;
  void** array;
  struct binding *p;

  Assert(table);

  array = ALLOC((2*table->length + 1) * sizeof (*array));

  for (i = 0; i < table->size; i++)

    for (p = table->buckets[i]; p; p = p->link) {
      array[j++] = (void *)p->key;
      array[j++] = p->value;
    }

  array[j] = end;
  return array;
}
