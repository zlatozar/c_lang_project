#include <stdio.h>
#include <stdlib.h>

typedef int object_t;

typedef int key_t;

#define MAXP 46337  /* prime, and 46337*46337 < 2147483647 */

typedef struct {
  int          size;
  int     primary_a;
  int*  secondary_a;
  int*  secondary_s;
  int*  secondary_o;
  int*         keys;
  object_t*    objs;
} perf_hash_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256

perf_hash_t*
create_perf_hash(int size, int keys[], object_t objs[])
{
  perf_hash_t* tmp;
  int* table1, *table2, *table3, *table4;
  int i, j, k, collision, sq_bucket_sum, sq_sum_limit, a;
  object_t* objects;

  tmp = (perf_hash_t*) malloc( sizeof(perf_hash_t) );
  table1 = (int*) malloc( size * sizeof(int) );
  table2 = (int*) malloc( size * sizeof(int) );
  table3 = (int*) malloc( size * sizeof(int) );
  sq_sum_limit  =   5 * size;
  sq_bucket_sum = 100 * size;

  while (sq_bucket_sum > sq_sum_limit) { /* find primary factor */
    a = rand() % MAXP;

    for (i = 0; i < size; i++)
    { table1[i] = 0; }

    for (i = 0; i < size; i++)
    { table1[ (((a * keys[i]) % MAXP) % size) ] += 1; }

    sq_bucket_sum = 0;

    for (i = 0; i < size; i++)
    { sq_bucket_sum += table1[i] * table1[i]; }
  }

  /* compute secondary table sizes and their offset */
  for (i = 0; i < size; i++ ) {
    table1[i] = 2 * table1[i] * table1[i];
    table2[i] = (i > 0) ? table2[i - 1] + table1[i - 1] : 0;
  }

  table4 = (int*) malloc( 2 * sq_bucket_sum * sizeof(int) );

  for ( i = 0; i < 2 * sq_bucket_sum; i++ )
  { table4[i] = MAXP; } /* different from all keys */

  collision = 1;

  for ( i = 0; i < size; i++ )
  { table3[i] = rand() % MAXP; } /* secondary hash factor */

  while ( collision ) {

    collision = 0;
    for ( i = 0; i < size; i++ ) {
      j = ((keys[i] * a) % MAXP) % size;
      k = ((keys[i] * table3[j]) % MAXP) % table1[j] + table2[j];

      if ( table4[k] == MAXP || table4[k] == keys[i] )
      { table4[k] = keys[i]; } /* entry up to now empty */

      else { /* collision */
        collision = 1;
        table3[i] = 0; /* mark bucket as defect */
      }
    }

    if ( collision ) {

      for ( i = 0; i < size; i++) {

        if ( table3[i] == 0 ) { /* defect bucket */
          table3[i] = rand() % MAXP; /* choose new factor */

          for ( k = table2[i]; k < table2[i] + table1[i]; k++)
          { table4[k] = MAXP; } /* clear i-th secondary table */
        }
      }
    }
  } /* now the hash table is collision-free */

  /* keys are in the right places, now put objects there */
  objects = (object_t*)malloc(2 * sq_bucket_sum * sizeof(object_t) );
  for ( i = 0; i < size; i++ ) {
    j = ((keys[i] * a) % MAXP) % size;
    k = ((keys[i] * table3[j]) % MAXP) % table1[j] + table2[j];
    objects[k] = objs[i];
  }

  tmp->size = size;
  tmp->primary_a = a;        /* primary hash table factor */
  tmp->secondary_a = table3; /* secondary hash table factors */
  tmp->secondary_s = table1; /* secondary hash table sizes */
  tmp->secondary_o = table2; /* secondary hash table offsets */
  tmp->keys = table4; /* secondary hash tables */
  tmp->objs = objects;

  return ( tmp );
}

object_t*
find(perf_hash_t* ht, int query_key)
{
  int i, j;
  i = ((ht->primary_a * query_key) % MAXP) % ht->size;

  if ( ht->secondary_s[i] == 0 )
  { return ( NULL ); } /* secondary bucket empty */

  else {
    j = ((ht->secondary_a[i] * query_key) % MAXP) % ht->secondary_s[i]
        + ht->secondary_o[i];

    if ( ht->keys[j] == query_key )
    { return ( (ht->objs) + j ); } /* right key found */

    else
    { return ( NULL ); } /* query_key does not exist. */
  }
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  char nextop;
  int keys[1000];
  int objects[1000];
  int size = 0;
  int i;
  perf_hash_t* ht;
  printf("Enter Keys (here we choose object 10*k for key k)\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int inskey;
      scanf(" %d", &inskey);
      keys[size] = inskey;
      objects[size] = 10 * inskey;
      size += 1;
    }
  }

  printf("\nList of keys:\n");
  for ( i = 0; i < size ; i++ )
  { printf(" %d", keys[i] ); }

  printf("\n");
  ht = create_perf_hash( size, keys, objects );
  printf("created perfect hash table\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'f' ) {
      int findkey, *findobj;
      scanf(" %d", &findkey);
      printf(" looking for key %d\n", findkey);
      findobj = find( ht, findkey);

      if ( findobj == NULL )
      { printf("  find failed, for key %d\n", findkey); }

      else
      { printf("  find successful, found object %d\n", *findobj); }
    }
  }
  return (0);
}
