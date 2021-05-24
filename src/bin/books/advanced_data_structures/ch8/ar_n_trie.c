#include <stdio.h>
#include <stdlib.h>

typedef char object_t;

typedef struct trie_n_t {
  struct trie_n_t* next[256];
  /* possibly additional information*/
} trie_node_t;

typedef trie_node_t node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 100

node_t* currentblock = NULL;
int size_left;
node_t* free_list = NULL;

node_t*
get_node()
{
  node_t* tmp;

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
  free_list = node;
}

trie_node_t*
create_trie(void)
{
  trie_node_t* tmp_node;
  int i;
  tmp_node = get_node();

  for ( i = 0; i < 256; i++)
  { tmp_node->next[i] = NULL; }

  return ( tmp_node );
}

object_t*
find(trie_node_t* trie, char* query_string)
{
  trie_node_t* tmp_node;
  char* query_next;
  tmp_node =  trie;
  query_next = query_string;

  while (*query_next != '\0') {

    if ( tmp_node->next[(int)(*query_next)] == NULL)
    { return ( NULL ); } /* query string not found */

    else {
      tmp_node = tmp_node->next[(int)(*query_next)];
      query_next += 1; /* move to next character of query */
    }
  }

  return ((object_t*) tmp_node->next[(int)'\0']);
}

int
insert(trie_node_t* trie, char* new_string,
       object_t* new_object)
{
  trie_node_t* tmp_node, *new_node;
  char* query_next;
  int i;

  tmp_node =  trie;
  query_next = new_string;

  while (*query_next != '\0') {

    if ( tmp_node->next[(int)(*query_next)] == NULL) {
      new_node = get_node(); /* create missing node */

      for ( i = 0; i < 256; i++)
      { new_node->next[i] = NULL; }

      tmp_node->next[(int)(*query_next)] = new_node;
    }
    /* move to next character */
    tmp_node = tmp_node->next[(int)(*query_next)];
    query_next += 1; /* move to next character */
  }

  if ( tmp_node->next[(int)'\0'] != NULL )
  { return ( -1 ); } /* string already exists, has object */

  else
  { tmp_node->next[(int)'\0'] = (trie_node_t*) new_object; }

  return ( 0 );
}

object_t*
delete (trie_node_t* trie, char* delete_string)
{
  trie_node_t* tmp_node;
  object_t* tmp_object;
  char* next_char;
  int i;
  int finished = 0;
  trie_node_t* node_stack[100];
  int st_p = 0;

  tmp_node =  trie;
  next_char = delete_string;

  while (*next_char != '\0') {

    if ( tmp_node->next[(int)(*next_char)] == NULL)
    { return ( NULL ); } /* delete_string does not exist */

    else {
      tmp_node = tmp_node->next[(int)(*next_char)];
      next_char += 1; /* move to next character */
      node_stack[st_p++] = tmp_node;
    }
  }

  tmp_object = (object_t*) tmp_node->next[(int)'\0'];

  /* remove all nodes that became unnecessary */
  /* the root is not on the stack, so it is never deleted */
  while ( st_p > 0 && !finished ) {
    tmp_node = node_stack[ --st_p ];
    tmp_node->next[(int)(*next_char)] = NULL;

    for ( i = 0; i < 256; i++)
    { finished = finished || (tmp_node->next[i] != NULL ); }
    /* if  tmp_node is all NULL, it should be deleted */

    if ( !finished ) {
      return_node( tmp_node );
      next_char -= 1;
    }
  }
  return ( tmp_object );
}

void
remove_trie(trie_node_t* trie)
{
  trie_node_t* tmp_node;
  trie_node_t* node_stack[100];
  int st_p = 0;
  node_stack[st_p++] = trie;

  while ( st_p > 0 ) {
    int i;
    tmp_node = node_stack[ --st_p ];

    for ( i = 0; i < 256; i++) {
      if ( tmp_node->next[i] != NULL && i != (int)'\0' )
      { node_stack[st_p++] = tmp_node->next[i]; }
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
      int success;
      ins_obj = (char*) malloc(100 * sizeof(char));
      scanf(" %s %s", ins_key, ins_obj);
      success = insert( tr, ins_key, ins_obj );

      if ( success == 0 )
        printf("  insert successful, key = %s, object = %s\n",
               ins_key, ins_obj);

      else
      { printf("  insert failed, success = %d\n", success); }
    }

    if ( nextop == 'f' ) {
      char find_key[100], *find_obj;
      scanf(" %s", find_key);
      find_obj = find( tr, find_key);

      if ( find_obj == NULL )
      { printf("  find failed, for key %s\n", find_key); }

      else
      { printf("  find successful, found object %s\n", find_obj); }
    }

    if ( nextop == 'd' ) {
      char del_key[100], *del_obj;
      scanf(" %s", del_key);
      del_obj = delete ( tr, del_key);

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

  remove_trie(tr);
  printf(" removed trie structure \n");

  return (0);
}
