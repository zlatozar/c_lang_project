#include <stdio.h>
#include <stdlib.h>

typedef char object_t;

typedef struct trie_n_t {
  char        this_char;
  struct trie_n_t* next;
  struct trie_n_t* list;
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
    free_list = free_list -> next;

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
  node->next = free_list;
  free_list = node;
}

trie_node_t*
create_trie(void)
{
  trie_node_t* tmp_node;
  tmp_node = get_node();
  tmp_node->next = tmp_node->list = NULL;
  tmp_node->this_char = '\0';

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

    while ( tmp_node->this_char != *query_next ) {

      if ( tmp_node->list == NULL )
      { return ( NULL ); } /* query string not found */

      else
      { tmp_node  = tmp_node->list ; }
    }

    tmp_node = tmp_node->next;
    query_next += 1;
  }

  /* reached end of query string */
  while ( tmp_node->this_char != '\0' ) {

    if ( tmp_node->list == NULL )
    { return ( NULL ); } /* query string not found */

    else
    { tmp_node  = tmp_node->list ; }
  }

  return ( (object_t*) tmp_node->next);
}

int
insert(trie_node_t* trie, char* new_string, object_t* new_object)
{
  trie_node_t* tmp_node;
  char* query_next;
  int finished = 0;
  tmp_node =  trie;
  query_next = new_string;

  /* first go as far as possible in existing trie */
  while ( !finished ) {

    /* follow list till matching character is found */
    while ( tmp_node->this_char != *query_next
            &&  tmp_node->list != NULL )
    { tmp_node  = tmp_node->list ; }

    if ( tmp_node->this_char == *query_next  ) {

      /* matching character found, might be last */
      if ( *query_next != '\0' ) { /* not last. follow */
        tmp_node = tmp_node->next;
        query_next += 1;

      } else /* insertion not possible, string already exists */
      { return ( -1 ); }

    } else
    { finished = 1; }
  }

  /* left existing trie, create new branch */
  tmp_node->list = get_node();
  tmp_node = tmp_node->list;
  tmp_node->list = NULL;
  tmp_node->this_char = *query_next;

  while ( *query_next != '\0') {
    query_next += 1;
    tmp_node->next = get_node();
    tmp_node = tmp_node->next;
    tmp_node->list = NULL;
    tmp_node->this_char = *query_next;
  }

  tmp_node->next = (trie_node_t*) new_object;
  return ( 0 );
}

object_t*
delete (trie_node_t* trie, char* delete_string)
{
  trie_node_t* tmp, *tmp_prev, *first_del, *last_undel;
  object_t* del_object;
  char* del_next;

  if ( trie->list == NULL || *delete_string == '\0' )
  { return ( NULL ); } /* delete failed: trie empty */

  else { /* trie not empty, can start */
    int finished = 0;
    int branch = 1;
    last_undel = tmp_prev = trie;
    first_del  = tmp = trie->list;
    del_next = delete_string;

    while ( !finished ) {

      while ( tmp->this_char != *del_next ) {

        /* follow list to find matching character */
        if ( tmp->list == NULL ) /* none found*/
        { return ( NULL ); }   /* deletion failed */

        else { /* branching trie node */
          tmp_prev = tmp;
          tmp = tmp->list;
          branch = 1;
        }
      } /* tmp has matching next character */

      if ( branch || (tmp->list != NULL) ) {
        /* update position where to start deleting */
        last_undel = tmp_prev;
        first_del  = tmp;
        branch = 0;
      }

      if ( *del_next == '\0' )
      { finished = 1; } /* found deletion string */

      else {
        del_next += 1;
        tmp_prev = tmp;
        tmp = tmp->next;
      }
    } /* reached the end. now unlink and delete path */

    del_object = (object_t*) tmp->next;
    tmp->next = NULL; /* unlink del_object */

    if ( first_del == last_undel->next )
    { last_undel->next = first_del->list; }

    else /* first_del == last_undel->list */
    { last_undel->list = first_del->list; }

    /* final path of nonbranching nodes unlinked */
    tmp = first_del;

    while ( tmp != NULL ) { /* follow path, return nodes */
      first_del = tmp->next;
      return_node( tmp );
      tmp  = first_del;
    }

    return ( del_object );
  }
}

void
remove_trie(trie_node_t* trie)
{
  trie_node_t* tmp_node;
  trie_node_t* node_stack[100];
  int stack_p = 0;
  node_stack[stack_p++] = trie ;

  while ( stack_p > 0 ) {
    tmp_node = node_stack[--stack_p] ;

    if ( tmp_node->this_char != '\0')
    { node_stack[ stack_p++ ] = tmp_node->next; }

    if ( tmp_node->list != NULL)
    { node_stack[ stack_p++ ] = tmp_node->list; }

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
