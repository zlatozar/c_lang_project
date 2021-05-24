#include <stdio.h>
#include <stdlib.h>

typedef int object_t;

typedef int key_t;

typedef struct l_node {
  key_t           key;
  object_t*       obj;
  struct l_node* next;
} list_node_t;

typedef struct {
  int a;
  int b;
  int size;
} hf_param_t;

typedef struct {
  int            size;
  list_node_t** table;
  int (*hash_function)(key_t, hf_param_t);
  hf_param_t hf_param;
} hashtable_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256
#define MAXP 46337 /* prime, and 46337*46337 < 2147483647 */

list_node_t* currentblock = NULL;
int size_left;
list_node_t* free_list = NULL;

list_node_t*
get_node()
{
  list_node_t* tmp;

  if ( free_list != NULL ) {
    tmp = free_list;
    free_list = free_list -> next;

  } else {

    if ( currentblock == NULL || size_left == 0) {
      currentblock = (list_node_t*) malloc( BLOCKSIZE * sizeof(list_node_t) );
      size_left = BLOCKSIZE;
    }

    tmp = currentblock++;
    size_left -= 1;
  }
  return ( tmp );
}

void
return_node(list_node_t* node)
{
  node->next = free_list;
  free_list = node;
}

hashtable_t*
create_hashtable(int size)
{
  hashtable_t* tmp;
  int i;
  int a, b;
  int universalhashfunction(key_t, hf_param_t);

  if ( size >= MAXP )
  { exit(-1); } /* should not be called with that large size */

  /* possibly initialize random number generator here */
  tmp = (hashtable_t*) malloc( sizeof(hashtable_t) );
  tmp->size = size;
  tmp->table = (list_node_t**)malloc( size * sizeof(list_node_t*));

  for (i = 0; i < size; i++)
  { (tmp->table)[i] = NULL; }

  tmp->hf_param.a = rand() % MAXP;
  tmp->hf_param.b = rand() % MAXP;
  tmp->hf_param.size = size;
  tmp->hash_function = universalhashfunction;

  return ( tmp );
}

int
universalhashfunction(key_t key, hf_param_t hfp)
{
  return ( ((hfp.a * key + hfp.b) % MAXP) % hfp.size );
}

object_t*
find(hashtable_t* ht, key_t query_key)
{
  int i;
  list_node_t* tmp_node;
  i = ht->hash_function(query_key, ht->hf_param );
  tmp_node = (ht->table)[i];

  while ( tmp_node != NULL && tmp_node->key != query_key )
  { tmp_node = tmp_node->next; }

  if ( tmp_node == NULL )
  { return ( NULL ); } /* not found */

  else
  { return ( tmp_node->obj ); } /* key found */
}

void
insert(hashtable_t* ht, key_t new_key, object_t* new_obj)
{
  int i;
  list_node_t* tmp_node;
  i = ht->hash_function(new_key, ht->hf_param );
  tmp_node = (ht->table)[i];

  /* insert in front */
  (ht->table)[i] = get_node();
  ((ht->table)[i])->next = tmp_node;
  ((ht->table)[i])->key  = new_key;
  ((ht->table)[i])->obj  = new_obj;
}

object_t*
delete (hashtable_t* ht, key_t del_key)
{
  int i;
  list_node_t* tmp_node;
  object_t* tmp_obj;
  i = ht->hash_function(del_key, ht->hf_param );
  tmp_node = (ht->table)[i];

  if ( tmp_node == NULL )
  { return ( NULL ); } /* list empty, delete failed */

  if ( tmp_node->key == del_key ) { /* if first in list */
    tmp_obj = tmp_node->obj;
    (ht->table)[i] = tmp_node->next;
    return_node( tmp_node );
    return ( tmp_obj );
  }

  /* list not empty, delete not first in list */
  while ( tmp_node->next != NULL
          &&  tmp_node->next->key != del_key )
  { tmp_node = tmp_node->next; }

  if ( tmp_node->next == NULL )
  { return ( NULL ); } /* not found, delete failed */

  else {
    list_node_t* tmp_node2; /* unlink node */
    tmp_node2 = tmp_node->next;
    tmp_node->next = tmp_node2->next;
    tmp_obj = tmp_node2->obj;
    return_node( tmp_node2 );

    return ( tmp_obj );
  }
}

object_t*
find_mtf(hashtable_t* ht, key_t query_key)
{
  int i;
  list_node_t* front_node, *tmp_node1, *tmp_node2;
  i = ht->hash_function(query_key, ht->hf_param );
  front_node = tmp_node1 = (ht->table)[i];
  tmp_node2 = NULL;

  while ( tmp_node1 != NULL && tmp_node1->key != query_key ) {
    tmp_node2 = tmp_node1;
    tmp_node1 = tmp_node1->next;
  }

  if ( tmp_node1 == NULL )
  { return ( NULL ); } /* not found */

  else { /* key found */

    if ( tmp_node1 != front_node ) { /* move to front */
      tmp_node2->next = tmp_node1->next; /* unlink */
      tmp_node1->next = front_node;
      (ht->table)[i] = tmp_node1;
    }

    return ( tmp_node1->obj );
  }
}

void
list_table(hashtable_t* ht)
{
  int i;
  list_node_t* tmp_node;

  for (i = 0; i < ht->size; i++ ) {
    printf("|");
    tmp_node = (ht->table)[i];

    while ( tmp_node != NULL ) {
      printf("%d ", (tmp_node->key) );
      tmp_node = tmp_node->next;
    }
  }
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  hashtable_t* ha;
  char nextop;
  ha = create_hashtable(20);
  printf("Made Hashtable\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int inskey,  *insobj;
      insobj = (int*) malloc(sizeof(int));
      scanf(" %d", &inskey);
      *insobj = 10 * inskey + 2;
      insert( ha, inskey, insobj );
      printf("  inserted key = %d, object value = %d\n", inskey, *insobj);
    }

    if ( nextop == 'f' ) {
      int findkey, *findobj;
      scanf(" %d", &findkey);
      findobj = find( ha, findkey);

      if ( findobj == NULL )
      { printf("  basic find failed, for key %d\n", findkey); }

      else
      { printf("  basic find successful, found object %d\n", *findobj); }

      findobj = find_mtf( ha, findkey);

      if ( findobj == NULL )
      { printf("  find (mtf) failed, for key %d\n", findkey); }

      else
      { printf("  find (mtf) successful, found object %d\n", *findobj); }
    }

    if ( nextop == 'd' ) {
      int delkey, *delobj;
      scanf(" %d", &delkey);
      delobj = delete ( ha, delkey);

      if ( delobj == NULL )
      { printf("  delete failed for key %d\n", delkey); }

      else
      { printf("  delete successful, deleted object %d\n", *delobj); }
    }

    if ( nextop == '?' ) {
      printf("  Checking table\n");
      list_table(ha);
      printf("  Finished Checking table\n");
    }
  }
  return (0);
}
