#include <stdio.h>
#include <stdlib.h>

typedef char object_t;

typedef struct l_node {
  char*           key;
  object_t*       obj;
  struct l_node* next;
} list_node_t;

typedef struct htp_l_node {
  int                   a;
  struct htp_l_node* next;
} htp_l_node_t;

typedef struct {
  int                     b;
  int                  size;
  struct htp_l_node* a_list;
} hf_param_t;

typedef struct {
  int            size;
  list_node_t** table;
  int (*hash_function)(char*, hf_param_t);
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
  int universalhashfunction(char*, hf_param_t);

  if ( size >= MAXP )
  { exit(-1); } /* should not be called with that large size */

  tmp = (hashtable_t*) malloc( sizeof(hashtable_t) );
  tmp->size = size;
  tmp->table = (list_node_t**)malloc( size * sizeof(list_node_t*));

  for (i = 0; i < size; i++)
  { (tmp->table)[i] = NULL; }

  tmp->hf_param.b = rand() % MAXP;
  tmp->hf_param.size = size;
  tmp->hf_param.a_list = (htp_l_node_t*) get_node();
  tmp->hf_param.a_list->next = NULL;
  tmp->hash_function = universalhashfunction;

  return ( tmp );
}

int
universalhashfunction(char* key, hf_param_t hfp)
{
  int sum;
  htp_l_node_t* al;
  sum = hfp.b;
  al = hfp.a_list;

  while ( *key != '\0' ) {

    if ( al->next == NULL ) {
      al->next = (htp_l_node_t*) get_node();
      al->next->next = NULL;
      al->a = rand() % MAXP;
    }

    sum += ( (al->a) * ((int) * key)) % MAXP;
    key += 1;
    al = al->next;
  }
  return ( sum % hfp.size );
}

object_t*
find(hashtable_t* ht, char* query_key)
{
  int i;
  list_node_t* tmp_node;
  char* tmp1, *tmp2;
  i = ht->hash_function(query_key, ht->hf_param );
  tmp_node = (ht->table)[i];

  while ( tmp_node != NULL ) {
    tmp1 = tmp_node->key;
    tmp2 = query_key;

    while ( *tmp1 != '\0' && *tmp2 != '\0' && *tmp1 == *tmp2 )
    {  tmp1++; tmp2++; }

    if ( *tmp1 != *tmp2 ) /*strings not equal */
    { tmp_node = tmp_node->next; }

    else /* strings equal: correct entry found */
    { break; }
  }

  if ( tmp_node == NULL )
  { return ( NULL ); }           /* not found */

  else
  { return ( tmp_node->obj ); }  /* key found */
}

void
insert(hashtable_t* ht, char* new_key, object_t* new_obj)
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
delete (hashtable_t* ht, char* del_key)
{
  int i;
  list_node_t* tmp_node;
  object_t* tmp_obj;
  char* tmp1, *tmp2;
  i = ht->hash_function(del_key, ht->hf_param );
  tmp_node = (ht->table)[i];

  if ( tmp_node == NULL )
  { return ( NULL ); } /* list empty, delete failed */

  /* test first item in list */
  tmp1 = tmp_node->key;
  tmp2 = del_key;

  while ( *tmp1 != '\0' && *tmp2 != '\0' && *tmp1 == *tmp2 )
  {  tmp1++; tmp2++; }

  if ( *tmp1 == *tmp2 ) { /* strings equal: correct entry found */
    tmp_obj = tmp_node->obj; /* delete first entry in list */
    (ht->table)[i] = tmp_node->next;
    return_node( tmp_node );

    return ( tmp_obj );
  }

  /* list not empty, delete not first in list */
  while ( tmp_node->next != NULL ) {
    tmp1 = tmp_node->next->key;
    tmp2 = del_key;

    while ( *tmp1 != '\0' && *tmp2 != '\0' && *tmp1 == *tmp2 )
    {  tmp1++; tmp2++; }

    if ( *tmp1 != *tmp2 ) /* strings not equal */
    { tmp_node = tmp_node->next; }

    else /* strings equal: correct entry found */
    { break; }
  }

  if ( tmp_node->next == NULL )
  { return ( NULL ); } /* not found, delete failed */

  else {
    list_node_t* tmp_node2; /* unlink node */
    tmp_node2 = tmp_node->next;
    tmp_node->next = tmp_node->next->next;
    tmp_obj = tmp_node2->obj;
    return_node( tmp_node2 );

    return ( tmp_obj );
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
      printf("%s ", (tmp_node->key) );
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
      char* ins_key, *ins_obj;
      int success;
      ins_key = (char*) malloc(100 * sizeof(char));
      ins_obj = (char*) malloc(100 * sizeof(char));
      scanf(" %s %s", ins_key, ins_obj);
      insert( ha, ins_key, ins_obj );
      printf("  inserted key = %s, object = %s\n", ins_key, ins_obj);
    }

    if ( nextop == 'f' ) {
      char find_key[100], *find_obj;
      scanf(" %s", find_key);
      find_obj = find( ha, find_key);

      if ( find_obj == NULL )
      { printf("  find failed, for key %s\n", find_key); }

      else
      { printf("  find successful, found object %s\n", find_obj); }
    }

    if ( nextop == 'd' ) {
      char del_key[100], *del_obj;
      scanf(" %s", del_key);
      del_obj = delete ( ha, del_key);

      if ( del_obj == NULL )
      { printf("  delete failed for key %s\n", del_key); }

      else
      { printf("  delete successful, deleted object %s,\n", del_obj); }
    }

    if ( nextop == '?' ) {
      printf("  Checking Hashtable: listing keys\n");
      list_table(ha);
      printf("\n  Finished Checking Hashtable\n");
    }
  }

  return (0);
}
