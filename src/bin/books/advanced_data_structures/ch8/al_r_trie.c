#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char object_t;

typedef struct trie_n_t {
  struct trie_n_t* next[16];
  object_t*          object;
  int       reference_count;
  /* possibly additional information*/
} trie_node_t;

typedef trie_node_t node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 100

int node_given = 0, node_returned = 0;
node_t* currentblock = NULL;
int size_left;
node_t* free_list = NULL;

node_t*
get_node()
{
  node_t* tmp;
  node_given += 1;

  if ( free_list != NULL ) {
    tmp = free_list;
    free_list = free_list -> next[0];

  } else {

    if ( currentblock == NULL || size_left == 0) {
      currentblock = (node_t*) malloc( BLOCKSIZE * sizeof(node_t) );
      size_left = BLOCKSIZE;
    }

    tmp = currentblock++;
    size_left -= 1;
  }
  return ( tmp );
}

void
return_node(node_t* node)
{
  node->next[0] = free_list;
  node_returned += 1;
  free_list = node;
}

trie_node_t*
create_trie(void)
{
  trie_node_t* tmp_node;
  int i;
  tmp_node = get_node();

  for ( i = 0; i < 16; i++ )
  { tmp_node->next[i] = NULL; }

  tmp_node->object = NULL;
  tmp_node->reference_count = 1; /* root cannot be deleted */

  return ( tmp_node );
}

object_t*
find(trie_node_t* trie, char* query_string, int query_length)
{
  trie_node_t* tmp1_node, *tmp2_node;
  int query_pos;

  tmp1_node =  trie;
  for ( query_pos = 0; query_pos < query_length; query_pos ++) {
    tmp2_node =
      tmp1_node->next[((((int)query_string[query_pos]) & 0xF0) >> 4)];

    if ( tmp2_node != NULL )
    { tmp1_node = tmp2_node; } /* used upper four bits */

    else
    { return ( NULL ); } /* query string not found */

    tmp2_node =
      tmp1_node->next[((int)query_string[query_pos]) & 0x0F];

    if ( tmp2_node != NULL )
    { tmp1_node = tmp2_node; } /*used lower four bits */

    else
    { return ( NULL ); } /* query string not found */
  }

  /* reached end of query string */
  return ( tmp1_node->object); /* NULL if query string not found */
}

int
insert(trie_node_t* trie, char* new_string, int new_length, object_t* new_object)
{
  trie_node_t* tmp1_node, *tmp2_node;
  int current_pos;
  int next_sub_char;
  tmp1_node =  trie;

  for ( current_pos = 0; current_pos < 2 * new_length; current_pos++) {

    if ( current_pos % 2 == 0 ) /* use upper four bits next */
      next_sub_char =
        (((int)new_string[current_pos / 2]) & 0xF0) >> 4;

    else /* use lower four bits next */
    { next_sub_char = ((int)new_string[current_pos / 2]) & 0x0F; }

    tmp2_node = tmp1_node->next[ next_sub_char ];

    if ( tmp2_node != NULL )
    { tmp1_node = tmp2_node; } /* used four bits */

    else { /* need to create new node */
      int i;
      tmp2_node = get_node();

      for ( i = 0; i < 16; i++ )
      { tmp2_node->next[i] = NULL; }

      tmp2_node->object = NULL;
      tmp2_node->reference_count = 0;
      tmp1_node->next[ next_sub_char ] = tmp2_node;
      tmp1_node->reference_count += 1;
      tmp1_node = tmp2_node;
    }
  }

  if ( tmp1_node->object != NULL )
  { return ( -1); } /* string already exists, has associated object*/

  else {
    tmp1_node->object = new_object;
    tmp1_node->reference_count += 1;
  }

  return ( 0 );
}

object_t*
delete (trie_node_t* trie, char* del_string, int del_length)
{
  trie_node_t* tmp1_node, *tmp2_node;
  int current_pos;
  int next_sub_char;
  trie_node_t* del_start_node;
  int del_start_pos;
  object_t* tmp_object;

  tmp1_node =  trie;
  del_start_node = trie;
  del_start_pos = 0;

  for ( current_pos = 0; current_pos < 2 * del_length; current_pos++) {

    if ( current_pos % 2 == 0 ) /* use upper four bits next */
      next_sub_char =
        (((int)del_string[current_pos / 2]) & 0xF0) >> 4;

    else /* use lower four bits next */
    { next_sub_char = ((int)del_string[current_pos / 2]) & 0x0F; }

    tmp2_node = tmp1_node->next[ next_sub_char ];

    if ( tmp2_node != NULL ) {

      if ( tmp1_node->reference_count > 1 ) {
        del_start_node = tmp1_node;
        del_start_pos = current_pos;
      }  /* del_start_node is the last node with two pointers */

      tmp1_node = tmp2_node; /* used four bits */

    } else
    { return ( NULL ); } /* delete_string did not exist */
  }

  if ( tmp1_node->object == NULL )
  { return ( NULL ); } /* delete_string did not exist */

  else {
    tmp1_node->reference_count -= 1;
    tmp_object = tmp1_node->object;
    tmp1_node->object = NULL;
  }

  if ( tmp1_node->reference_count == 0) {

    tmp1_node = del_start_node;
    for ( current_pos = del_start_pos;
          current_pos < 2 * del_length; current_pos++) {

      if ( current_pos % 2 == 0 ) /* use upper four bits next */
        next_sub_char
          = (((int)del_string[current_pos / 2]) & 0xF0) >> 4;

      else /* use lower four bits next */
      { next_sub_char = ((int)del_string[current_pos / 2]) & 0x0F; }

      tmp2_node = tmp1_node->next[ next_sub_char ];
      tmp1_node->next[ next_sub_char ] = NULL;
      tmp1_node->reference_count -= 1;

      if ( tmp1_node->reference_count == 0 )
      { return_node( tmp1_node ); }

      tmp1_node = tmp2_node;
    }
    return_node( tmp1_node );
  }

  return ( tmp_object );
}

void
remove_trie(trie_node_t* trie)
{
  trie_node_t* tmp_node, *node_stack[100];
  int st_p = 0;
  node_stack[ st_p++] = trie;

  while ( st_p > 0 ) {
    int i;
    tmp_node = node_stack[ --st_p ];

    for ( i = 0; i < 16; i++) {
      if ( tmp_node->next[i] != NULL )
      { node_stack[ st_p++ ] = tmp_node->next[i] ; }
    }
    return_node( tmp_node );
  }
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  trie_node_t* tr;
  char nextop;
  tr = create_trie();
  printf("Made Trie\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      char ins_key[100], *ins_obj;
      int ins_length, success;
      ins_obj = (char*) malloc(100 * sizeof(char));
      scanf(" %s %s", ins_key, ins_obj);
      ins_length = strlen(ins_key);
      success = insert( tr, ins_key, ins_length, ins_obj );

      if ( success == 0 )
        printf("  insert successful, key = %s, object = %s\n",
               ins_key, ins_obj);
      else
      { printf("  insert failed, success = %d\n", success); }
    }

    if ( nextop == 'f' ) {
      char find_key[100], *find_obj;
      int find_length;
      scanf(" %s", find_key);
      find_length = strlen( find_key );
      find_obj = find( tr, find_key, find_length);

      if ( find_obj == NULL )
      { printf("  find failed, for key %s\n", find_key); }

      else
      { printf("  find successful, found object %s\n", find_obj); }
    }

    if ( nextop == 'd' ) {
      char del_key[100], *del_obj;
      int del_length;
      scanf(" %s", del_key);
      del_length = strlen( del_key );
      del_obj = delete ( tr, del_key, del_length);

      if ( del_obj == NULL )
      { printf("  delete failed for key %s\n", del_key); }

      else
      { printf("  delete successful, deleted object %s,\n", del_obj); }
    }
    /*     if( nextop == '?' )
    {  printf("  Checking tree\n");
       check_tree(searchtree,0,-1000,1000);
       printf("\n");
       if( searchtree->left != NULL )
    printf("key in root is %d, height of tree is %d\n",
    searchtree->key, searchtree->height );
       printf("  Finished Checking tree\n");
    }*/
  }
  printf("nodes given out: %d. nodes returned: %d. Now removing trie\n",
         node_given, node_returned );

  remove_trie(tr);

  printf(" removed trie structure. ");
  printf("nodes given out: %d. nodes returned: %d\n",
         node_given, node_returned );

  return (0);
}
