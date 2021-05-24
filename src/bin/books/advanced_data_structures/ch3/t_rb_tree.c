#include <stdio.h>
#include <stdlib.h>

typedef int object_t;

typedef int key_t;

typedef struct tr_n_t {
  key_t             key;
  struct tr_n_t*      left;
  struct tr_n_t*     right;
  enum {red, black}  color;
  /* possibly other information */
} tree_node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256

tree_node_t* currentblock = NULL;
int size_left;
tree_node_t* free_list = NULL;

tree_node_t*
get_node()
{
  tree_node_t* tmp;

  if ( free_list != NULL ) {
    tmp = free_list;
    free_list = free_list -> left;

  } else {
    if ( currentblock == NULL || size_left == 0) {
      currentblock = (tree_node_t*) malloc( BLOCKSIZE * sizeof(tree_node_t) );
      size_left = BLOCKSIZE;
    }

    tmp = currentblock++;
    size_left -= 1;
  }
  return ( tmp );
}

void
return_node(tree_node_t* node)
{
  node->left = free_list;
  free_list = node;
}

tree_node_t*
create_tree(void)
{
  tree_node_t* tmp_node;

  tmp_node = get_node();
  tmp_node->left = NULL;
  tmp_node->color = black;

  return ( tmp_node );
}

void
left_rotation(tree_node_t* n)
{
  tree_node_t* tmp_node;
  key_t        tmp_key;

  tmp_node = n->left;
  tmp_key  = n->key;

  n->left  = n->right;
  n->key   = n->right->key;
  n->right = n->left->right;
  n->left->right = n->left->left;
  n->left->left  = tmp_node;
  n->left->key   = tmp_key;
}

void
right_rotation(tree_node_t* n)
{
  tree_node_t* tmp_node;
  key_t        tmp_key;

  tmp_node = n->right;
  tmp_key  = n->key;

  n->right = n->left;
  n->key   = n->left->key;
  n->left  = n->right->left;
  n->right->left = n->right->right;
  n->right->right  = tmp_node;
  n->right->key   = tmp_key;
}

object_t*
find(tree_node_t* tree, key_t query_key)
{
  tree_node_t* tmp_node;

  if ( tree->left == NULL )
  { return (NULL); }

  else {
    tmp_node = tree;

    while ( tmp_node->right != NULL ) {
      if ( tmp_node->color == red ) { printf("r"); }

      else { printf("b"); }

      if ( query_key < tmp_node->key )
      { tmp_node = tmp_node->left; }

      else
      { tmp_node = tmp_node->right; }
    }

    if ( tmp_node->color == red ) { printf("r\n"); }

    else { printf("b\n"); }

    if ( tmp_node->key == query_key )
    { return ( (object_t*) tmp_node->left ); }

    else
    { return ( NULL ); }
  }
}

int
insert(tree_node_t* tree, key_t new_key, object_t* new_object)
{
  if ( tree->left == NULL ) {
    tree->left = (tree_node_t*) new_object;
    tree->key  = new_key;
    tree->color = black; /* root is always black */
    tree->right  = NULL;

  } else {

    tree_node_t* current, *next_node, *upper;
    current = tree;
    upper = NULL;

    while ( current->right != NULL ) {

      if ( new_key < current->key )
      { next_node = current->left; }

      else
      { next_node = current->right; }

      if ( current->color == black ) {

        if ( current->left->color == black || current->right->color == black ) {
          upper = current;
          current = next_node;

        } else { /* current->left and current->right red */

          /* need rebalance */
          if ( upper == NULL ) { /* current is root */
            current->left->color = black;
            current->right->color = black;
            upper = current;

          } else if (current->key < upper->key ) {

            /* current left of upper */
            if ( current == upper->left ) {
              current->left->color = black;
              current->right->color = black;
              current->color = red;

            } else if ( current == upper->left->left ) {
              right_rotation( upper );
              upper->left->color = red;
              upper->right->color = red;
              upper->left->left->color = black;
              upper->left->right->color = black;

            } else { /* current == upper->left->right */

              left_rotation( upper->left );
              right_rotation( upper );
              upper->left->color = red;
              upper->right->color = red;
              upper->right->left->color = black;
              upper->left->right->color = black;
            }

          } else { /* current->key >= upper->key */

            /* current right of upper */
            if ( current == upper->right ) {
              current->left->color = black;
              current->right->color = black;
              current->color = red;

            } else if ( current == upper->right->right ) {
              left_rotation( upper );
              upper->left->color = red;
              upper->right->color = red;
              upper->right->left->color = black;
              upper->right->right->color = black;

            } else { /* current == upper->right->left */
              right_rotation( upper->right );
              left_rotation( upper );
              upper->left->color = red;
              upper->right->color = red;
              upper->right->left->color = black;
              upper->left->right->color = black;
            }

          } /* end rebalancing */

          current = next_node;
          upper = current; /*two black lower neighbors*/
        }

      } else { /* current red */
        current = next_node; /*move down */
      }

    } /* end while; reached leaf. always arrive on black leaf*/

    /* found the candidate leaf. Test whether key distinct */
    if ( current->key == new_key )
    { return ( -1 ); }

    /* key is distinct, now perform the insert */
    {
      tree_node_t* old_leaf, *new_leaf;

      old_leaf = get_node();
      old_leaf->left = current->left;
      old_leaf->key = current->key;
      old_leaf->right  = NULL;
      old_leaf->color = red;

      new_leaf = get_node();
      new_leaf->left = (tree_node_t*) new_object;
      new_leaf->key = new_key;
      new_leaf->right  = NULL;
      new_leaf->color = red;

      if ( current->key < new_key ) {
        current->left  = old_leaf;
        current->right = new_leaf;
        current->key = new_key;

      } else {
        current->left  = new_leaf;
        current->right = old_leaf;
      }
    }
  }
  return ( 0 );
}

object_t*
delete (tree_node_t* tree, key_t delete_key)
{
  object_t* deleted_object;
  int finished = 0;

  if ( tree->left == NULL )
  { return ( NULL ); }

  else if ( tree->right == NULL ) {

    if (  tree->key == delete_key ) {
      deleted_object = (object_t*) tree->left;
      tree->left = NULL;
      return ( deleted_object );

    } else
    { return ( NULL ); }

  } else {

    tree_node_t* current, *upper;
    tree->color = black; /* root is always black*/
    upper = tree;

    if ( upper->left->color == black &&
         upper->right->color == black ) {

      /* need to give upper ared lower neighbor */
      if ( delete_key < upper->key ) {

        if ( upper->left->right == NULL ) {

          if ( upper->right->right == NULL ) {
            upper->left->color  = red;
            upper->right->color = red;

          } else {
            upper->right->left->color  = black;
            upper->right->right->color = black;
            upper->right->color = red;
          }

        } else {

          if ( upper->left->left->color == red ||
               upper->left->right->color == red )
          { upper = upper->left; }

          else if ( upper->right->right->color == red ) {

            left_rotation( upper );
            upper->right->color = black;
            upper->left->color  = black;
            upper->left->left->color = red;
            upper = upper->left;

          } else if ( upper->right->left->color == red ) {

            right_rotation( upper->right );
            left_rotation( upper );
            upper->right->color = black;
            upper->left->color  = black;
            upper->left->left->color = red;
            upper = upper->left;

          } else { /* left and right have only black lower neighbors */
            upper->left->color = red;
            upper->right->color = red;
          }
        }

      } else { /* delete_key >= upper->key */

        if ( upper->right->left == NULL ) {

          if ( upper->left->right == NULL ) {
            upper->left->color  = red;
            upper->right->color = red;

          } else {
            upper->left->left->color  = black;
            upper->left->right->color = black;
            upper->left->color = red;
          }

        } else {

          if ( upper->right->right->color == red ||
               upper->right->left->color == red )
          { upper = upper->right; }

          else if ( upper->left->left->color == red ) {

            right_rotation( upper );
            upper->right->color = black;
            upper->left->color  = black;
            upper->right->right->color = red;
            upper = upper->right;

          } else if ( upper->left->right->color == red ) {
            left_rotation( upper->left );
            right_rotation( upper );
            upper->right->color = black;
            upper->left->color  = black;
            upper->right->right->color = red;
            upper = upper->right;

          } else { /* left and right have only black lower neighbors */
            upper->left->color = red;
            upper->right->color = red;
          }
        }
      }
    }  /* upper has at least one red lower neighbor */

    current = upper;

    while ( current->right != NULL ) {

      if ( delete_key < current->key )
      { current = current->left; }

      else
      { current = current->right; }

      if ( current->color == red || current->right == NULL )
      { continue; } /* go on down, or stop if leaf found */

      else { /* current->color == black, not leaf, rebalance */

        if ( current->left->color  == red ||
             current->right->color == red ) {
          upper = current; /* condition satisfied */

        } else { /* current->left and current->right black */

          /* need rebalance */
          if (current->key < upper->key ) {

            /* Case 2: current left of upper */
            if ( current == upper->left ) {

              if ( upper->right->left->left->color == black &&
                   upper->right->left->right->color == black ) {

                /* Case 2.1.1 */
                left_rotation( upper );
                upper->left->color = black;
                upper->left->left->color = red;
                upper->left->right->color = red;
                current = upper = upper->left;

              } else if ( upper->right->left->left->color == red ) {
                /* Case 2.1.2 */
                right_rotation( upper->right->left );
                right_rotation( upper->right );
                left_rotation( upper );
                upper->left->color = black;
                upper->right->left->color = black;
                upper->right->color = red;
                upper->left->left->color = red;
                current = upper = upper->left;

              } else /* upper->right->left->left->color == black &&
           upper->right->left->right->color == red */
              {  /* Case 2.1.3 */

                right_rotation( upper->right );
                left_rotation( upper );
                upper->left->color = black;
                upper->right->left->color = black;
                upper->right->color = red;
                upper->left->left->color = red;
                current = upper = upper->left;
              }
            } /*end Case 2.1: current==upper->left */

            else if ( current == upper->left->left ) {

              if ( upper->left->right->left->color == black &&
                   upper->left->right->right->color == black ) {
                /* Case 2.2.1 */
                upper->left->color = black;
                upper->left->left->color = red;
                upper->left->right->color = red;
                current = upper = upper->left;

              } else if ( upper->left->right->right->color == red ) {
                /* Case 2.2.2 */
                left_rotation( upper->left );
                upper->left->left->color = black;
                upper->left->right->color = black;
                upper->left->color = red;
                upper->left->left->left->color = red;
                current = upper = upper->left->left;
              } else /* upper->left->right->left->color == red &&
           upper->left->right->right->color == black */

              {  /* Case 2.2.3 */
                right_rotation( upper->left->right );
                left_rotation( upper->left );
                upper->left->left->color = black;
                upper->left->right->color = black;
                upper->left->color = red;
                upper->left->left->left->color = red;
                current = upper = upper->left->left;
              }

            } /*end Case 2.2: current==upper->left->left */

            else { /* current == upper->left->right */

              if ( upper->left->left->left->color == black &&
                   upper->left->left->right->color == black ) {

                /* Case 2.3.1 */
                upper->left->color = black;
                upper->left->left->color = red;
                upper->left->right->color = red;
                current = upper = upper->left;

              } else if ( upper->left->left->left->color == red ) {
                /* Case 2.3.2 */
                right_rotation( upper->left );
                upper->left->left->color = black;
                upper->left->right->color = black;
                upper->left->color = red;
                upper->left->right->right->color = red;
                current = upper = upper->left->right;

              } else /* upper->left->left->left->color == black &&
           upper->left->left->right->color == red */

              {  /* Case 2.3.3 */
                left_rotation( upper->left->left );
                right_rotation( upper->left );
                upper->left->left->color = black;
                upper->left->right->color = black;
                upper->left->color = red;
                upper->left->right->right->color = red;
                current = upper = upper->left->right;
              }
            } /*end Case 2.3: current==upper->left->right */
          } /* end Case 2: current->key < upper-> key */

          else {

            /* Case 3: current->key >= upper->key */
            if ( current == upper->right ) {

              if ( upper->left->right->right->color == black &&
                   upper->left->right->left->color == black ) {
                /* Case 3.1.1 */
                right_rotation( upper );
                upper->right->color = black;
                upper->right->right->color = red;
                upper->right->left->color = red;
                current = upper = upper->right;

              } else if ( upper->left->right->right->color == red ) {
                /* Case 3.1.2 */
                left_rotation( upper->left->right );
                left_rotation( upper->left );
                right_rotation( upper );
                upper->right->color = black;
                upper->left->right->color = black;
                upper->left->color = red;
                upper->right->right->color = red;
                current = upper = upper->right;

              } else /* upper->left->right->right->color == black &&
           upper->left->right->left->color == red */
              {  /* Case 3.1.3 */
                left_rotation( upper->left );
                right_rotation( upper );
                upper->right->color = black;
                upper->left->right->color = black;
                upper->left->color = red;
                upper->right->right->color = red;
                current = upper = upper->right;

              }
            } /*end Case 3.1: current==upper->left */

            else if ( current == upper->right->right ) {

              if ( upper->right->left->right->color == black &&
                   upper->right->left->left->color == black ) {
                /* Case 3.2.1 */
                upper->right->color = black;
                upper->right->left->color = red;
                upper->right->right->color = red;
                current = upper = upper->right;

              } else if ( upper->right->left->left->color == red ) {
                /* Case 3.2.2 */
                right_rotation( upper->right );
                upper->right->left->color = black;
                upper->right->right->color = black;
                upper->right->color = red;
                upper->right->right->right->color = red;
                current = upper = upper->right->right;

              } else /* upper->right->left->right->color == red &&
           upper->right->left->left->color == black */
              {  /* Case 3.2.3 */
                left_rotation( upper->right->left );
                right_rotation( upper->right );
                upper->right->right->color = black;
                upper->right->left->color = black;
                upper->right->color = red;
                upper->right->right->right->color = red;
                current = upper = upper->right->right;
              }
            } /*end Case 3.2: current==upper->right->right */

            else { /* current == upper->right->left */

              if ( upper->right->right->right->color == black &&
                   upper->right->right->left->color == black ) {

                /* Case 3.3.1 */
                upper->right->color = black;
                upper->right->left->color = red;
                upper->right->right->color = red;
                current = upper = upper->right;

              } else if ( upper->right->right->right->color == red ) {
                /* Case 3.3.2 */
                left_rotation( upper->right );
                upper->right->left->color = black;
                upper->right->right->color = black;
                upper->right->color = red;
                upper->right->left->left->color = red;
                current = upper = upper->right->left;

              } else /* upper->right->right->right->color == black &&
           upper->right->right->left->color == red */
              {  /* Case 3.3.3 */
                right_rotation( upper->right->right );
                left_rotation( upper->right );
                upper->right->left->color = black;
                upper->right->right->color = black;
                upper->right->color = red;
                upper->right->left->left->color = red;
                current = upper = upper->right->left;
              }
            } /*end Case 3.3: current==upper->right->left */
          } /* end Case 3: current->key >= upper-> key */
        } /* end rebalance, upper has a red lower neighbor */
      }
    }  /* end while */

    /* found the candidate leaf. Test whether key correct */
    if ( current->key != delete_key )
    { return ( NULL ); }

    else { /* want to delete node current */
      tree_node_t* tmp_node;
      deleted_object = (object_t*) current->left;

      if ( current->key < upper->key ) {

        if ( current == upper->left ) {
          /* upper->right is red */
          tmp_node = upper->right;
          upper->key   = tmp_node->key;
          upper->left  = tmp_node->left;
          upper->right = tmp_node->right;

        } else if ( current == upper->left->left ) {
          /* upper->left is red */
          tmp_node = upper->left;
          upper->left  = tmp_node->right;

        } else { /* current == upper->left->right */
          /* upper->left is red */
          tmp_node = upper->left;
          upper->left  = tmp_node->left;
        }

      } /* end current->key < upper->key */
      else { /* leaf to the right, current->key >= upper->key */

        if ( current == upper->right ) {
          /* upper->left is red */
          tmp_node = upper->left;
          upper->key   = tmp_node->key;
          upper->left  = tmp_node->left;
          upper->right = tmp_node->right;

        } else if ( current == upper->right->right ) {
          /* upper->right is red */
          tmp_node = upper->right;
          upper->right  = tmp_node->left;

        } else { /* current == upper->right->left */
          /* upper->right is red */
          tmp_node = upper->right;
          upper->right  = tmp_node->right;
        }
      } /* end current->key >= upper->key */

      return_node( tmp_node );
      return_node( current );
    }

    return ( deleted_object );
  }
}

void
check_tree(tree_node_t* tr, int count, int lower, int upper)
{
  if ( tr->left == NULL )
  {  printf("Tree Empty\n"); return; }

  if ( tr->key < lower || tr->key >= upper ) {
    printf("Wrong Key Order: node key %d, lower %d, upper %d \n",
           tr->key, lower, upper);
  }

  if ( tr->right == NULL ) { /* leaf */

    if ( *( (int*) tr->left) == 10 * tr->key + 2 )
    { printf("%d(%d)  ", tr->key, count + (tr->color == black) ); }
    else
    { printf("Wrong Object \n"); }

  } else { /* not leaf */

    if ( tr->color == red ) {

      if (  tr->left->color == red )
      { printf("wrong color below red key %d on the left\n", tr->key ); }

      if (  tr->right->color == red )
      { printf("wrong color below red key %d on the right\n", tr->key ); }
    }

    check_tree(tr->left, count + (tr->color == black), lower, tr->key );
    check_tree(tr->right, count + (tr->color == black), tr->key, upper );
  }
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  tree_node_t* searchtree;
  char nextop;
  searchtree = create_tree();
  printf("Made Tree: Red-Black Tree with Top-Down Rebalancing\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int inskey,  *insobj, success;
      insobj = (int*) malloc(sizeof(int));
      scanf(" %d", &inskey);
      *insobj = 10 * inskey + 2;
      success = insert( searchtree, inskey, insobj );

      if ( success == 0 )
        printf("  insert successful, key = %d, object value = %d,\n",
               inskey, *insobj );

      else
      { printf("  insert failed, success = %d\n", success); }
    }

    if ( nextop == 'f' ) {
      int findkey, *findobj;
      scanf(" %d", &findkey);
      findobj = find( searchtree, findkey);

      if ( findobj == NULL )
      { printf("  find failed, for key %d\n", findkey); }

      else
      { printf("  find successful, found object %d\n", *findobj); }
    }

    if ( nextop == 'd' ) {
      int delkey, *delobj;
      scanf(" %d", &delkey);
      delobj = delete ( searchtree, delkey);

      if ( delobj == NULL )
      { printf("  delete failed for key %d\n", delkey); }

      else
        printf("  delete successful, deleted object %d\n",
               *delobj);
    }

    if ( nextop == '?' ) {
      printf("  Checking tree\n");
      check_tree(searchtree, 0, -100000, 100000);
      printf("\n");

      if ( searchtree->left != NULL )
      { printf("key in root is %d,\n", searchtree->key); }

      printf("  Finished Checking tree\n");
    }
  }

  return (0);
}
