typedef struct nd_t {
  struct nd_t* next;
  /*and other components*/
} node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256

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
