#include <stdio.h>
#include <stdlib.h>

typedef int key_t;

typedef int object_t;

#define DIMENSION 2

typedef struct {
  key_t    coordinate[DIMENSION];
  object_t object;
} point_t;

typedef struct p_ls_n_t {
  struct p_ls_n_t* next;
  point_t*         point;
} p_list_node_t;

typedef struct tr_n_t {
  key_t                 key;
  struct tr_n_t*       left;
  struct tr_n_t*      right;
  struct tr_n_t* l_dim_tree;
  /* balancing information */
} tree_node_t;

// _____________________________________________________________________________

#define BLOCKSIZE 256
#define NEGINFTY -1000

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
      currentblock =
        (tree_node_t*) malloc( BLOCKSIZE * sizeof(tree_node_t ) );
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

p_list_node_t*
get_p_list_node()
{
  return ( ( p_list_node_t*) get_node() );
}

tree_node_t*
make_tree(tree_node_t* list)
{
  typedef struct {
    tree_node_t*  node1;
    tree_node_t*  node2;
    int          number;
  } st_item;

  st_item current, left, right;
  st_item stack[100];
  int st_p = 0;
  tree_node_t* tmp, *root;
  int length = 0;

  for ( tmp = list; tmp != NULL; tmp = tmp->right )
  { length += 1; } /* find length of list */

  root = get_node();
  current.node1 = root; /* put root node on stack */
  current.node2 = NULL;
  current.number = length;/* root expands to length leaves */
  stack[ st_p++ ] = current;

  while ( st_p > 0 ) { /* there is still unexpanded node */
    current = stack[ --st_p ];
    if ( current.number > 1 ) { /* create (empty) tree nodes */
      left.node1 = get_node();
      left.node2 = current.node2;
      left.number = current.number / 2;
      right.node1 = get_node();
      right.node2 = current.node1;
      right.number = current.number - left.number;
      (current.node1)->left  = left.node1;
      (current.node1)->right = right.node1;
      stack[ st_p++ ] = right;
      stack[ st_p++ ] = left;

    } else { /* reached a leaf, must be filled with list item */

      (current.node1)->left  = list->left;   /* fill leaf */
      (current.node1)->key   = list->key;    /* from list */
      (current.node1)->right = NULL;

      if ( current.node2 != NULL )
        /* insert comparison key in interior node */
      { (current.node2)->key   = list->key; }

      tmp = list;          /* unlink first item from list */
      list = list->right;  /* content has been copied to */
      return_node(tmp);    /* leaf, so node is returned */
    }
  }
  return ( root );
}

tree_node_t*
sort(tree_node_t* list)
{
  tree_node_t* tmp, *tmp2;  /* bubblesort on lists */
  tmp = tmp2 = NULL;
  int finished = 0;

  if ( list == NULL || list->right == NULL )
  { return ( list ); }

  while ( !finished ) {
    finished = 1;

    if (list->key > list->right->key ) {
      tmp = list->right;
      list->right = tmp->right;
      tmp->right = list;
      list = tmp;
      finished = 0;
    }

    tmp = list;
    while ( tmp->right->right != NULL ) {

      if (tmp->right->key > tmp->right->right->key ) {
        tmp2 = tmp->right->right;
        tmp->right->right = tmp2->right;
        tmp2->right = tmp->right;
        tmp->right  = tmp2;
        finished = 0;
      }
      tmp = tmp->right;
    }
  } /* sorted, now deal with duplicate values */

  tmp = list;
  while ( tmp->right != NULL ) {

    if ( tmp->key == tmp->right->key ) {
      p_list_node_t* pt1, *pt2; /* collect equal keys */
      pt1 = (p_list_node_t*) tmp->left;
      pt2 = (p_list_node_t*) tmp->right->left;
      pt2->next = pt1->next;
      pt1->next = pt2;
      tmp2 = tmp->right; /* and unlink node */
      tmp->right = tmp2->right;
      return_node( tmp2 );

    } else
    { tmp = tmp->right; }
  }

  return ( list );
}

tree_node_t*
build_or_r_tree(p_list_node_t* pt_list, int dim)
{
  if ( pt_list == NULL )
  { return ( NULL ); } /* should not be called for empty tree*/

  else {
    tree_node_t* o_tree, *t_tmp;
    tree_node_t* node_stack[100];
    int stack_p;
    tree_node_t* key_list, *k_tmp;
    p_list_node_t* p_tmp, *p_tmp2;
    /* create list of key values in dimension dim */
    key_list = NULL;
    p_tmp = pt_list;

    while ( p_tmp != NULL ) {
      k_tmp = get_node();
      k_tmp->key  = (p_tmp->point->coordinate)[dim];
      p_tmp2 = get_p_list_node();
      p_tmp2->point = p_tmp->point;
      p_tmp2->next  = NULL;
      k_tmp->left = (tree_node_t*) p_tmp2;
      k_tmp->right = key_list;
      key_list = k_tmp;
      p_tmp = p_tmp->next;
    }  /* made copy of point list */

    key_list = sort( key_list ); /* sort and remove duplicates*/

    if ( dim >= 1 ) { /* for interval decomposition, need -infty key*/
      k_tmp       = get_node();
      k_tmp->key  = NEGINFTY;
      k_tmp->right = key_list;
      k_tmp->left = NULL;
      key_list = k_tmp;
    }

    o_tree = make_tree( key_list );  /* create search tree */
    /* initialize all lower-dimensional trees to NULL */
    stack_p = 0;
    node_stack[stack_p++] = o_tree;

    while ( stack_p > 0 ) {
      t_tmp = node_stack[--stack_p];
      t_tmp->l_dim_tree = NULL;

      if ( t_tmp->right != NULL ) {
        node_stack[stack_p++] = t_tmp->left;
        node_stack[stack_p++] = t_tmp->right;
      }
    }

    if ( dim == 0 )
    { return ( o_tree ); } /* for dimension one: finished */

    else { /* need to construct lower-dimensional trees */
      /* insert each point, initially attach as list to nodes */

      while (  pt_list != NULL ) {
        t_tmp = o_tree; /* tree not empty */

        while ( t_tmp != NULL ) {
          p_tmp = get_p_list_node();
          p_tmp->next = (p_list_node_t*) t_tmp->l_dim_tree;
          p_tmp->point = pt_list->point;
          t_tmp->l_dim_tree = (tree_node_t*) p_tmp;

          if ( t_tmp->right != NULL &&
               pt_list->point->coordinate[dim] < t_tmp->key)
          { t_tmp = t_tmp->left; }

          else
          { t_tmp = t_tmp->right; }

        } /* attached point to each node on its search path */

        pt_list = pt_list->next; /* go to next point */
      }

      /* now create lower-dimensional trees for all nodes */
      stack_p = 0;
      node_stack[stack_p++] = o_tree;

      while ( stack_p > 0 ) {
        t_tmp = node_stack[--stack_p];

        if ( t_tmp->l_dim_tree != NULL )
          t_tmp->l_dim_tree = build_or_r_tree(
                                (p_list_node_t*) t_tmp->l_dim_tree, dim - 1);

        if ( t_tmp->right != NULL ) {
          node_stack[stack_p++] = t_tmp->left;
          node_stack[stack_p++] = t_tmp->right;
        }
      }
      /* finished */
      return ( o_tree );
    }
  }
}

p_list_node_t*
find_points_1d(tree_node_t* tree, key_t* a, key_t* b)
{
  tree_node_t* tr_node;
  tree_node_t* node_stack[200];
  int stack_p = 0;
  p_list_node_t* result_list, *tmp, *tmp2;
  result_list = NULL;
  node_stack[stack_p++] = tree;

  while ( stack_p > 0 ) {
    tr_node = node_stack[--stack_p];

    if ( tr_node->right == NULL ) {

      /* reached leaf, now test */
      if ( a[0] <= tr_node->key && tr_node->key < b[0] ) {

        /* must attach all points below this leaf */
        tmp = (p_list_node_t*) tr_node->left;

        while ( tmp != NULL ) {
          tmp2 = get_p_list_node();
          tmp2->point = tmp->point;
          tmp2->next  = result_list;
          result_list = tmp2;
          tmp = tmp->next;
        }
      }

    } else if ( b[0] <= tr_node->key )
    { node_stack[stack_p++] = tr_node->left; }

    else if ( tr_node->key <= a[0])
    { node_stack[stack_p++] = tr_node->right; }

    else {
      node_stack[stack_p++] = tr_node->left;
      node_stack[stack_p++] = tr_node->right;
    }
  }
  return ( result_list );
}

p_list_node_t*
join_list(p_list_node_t* a, p_list_node_t* b)
{
  if ( b == NULL )
  { return (a); }

  else {
    p_list_node_t* tmp;
    tmp = b;

    while ( tmp->next != NULL )
    { tmp = tmp->next; }

    tmp->next = a;

    return (b);
  }
}

p_list_node_t*
find_points(tree_node_t* tree, key_t* a, key_t* b, int dim)
{
  tree_node_t* current_node, *right_path, *left_path;
  p_list_node_t* current_list, *new_list;
  current_list = NULL;

  if ( tree->left == NULL )
  { exit(-1); } /* tree incorrect */

  else if ( dim == 0 )
  { return ( find_points_1d( tree, a, b ) ); }

  else {
    current_node = tree;
    right_path = left_path = NULL;

    while ( current_node->right != NULL ) { /* not at leaf */

      if ( b[dim] < current_node->key ) { /* go left: a < b < key */
        current_node = current_node->left;

      } else if ( current_node->key < a[dim])
        /* go right: key < b < a */
      {
        current_node = current_node->right;

      } else if ( a[dim] < current_node->key &&
                  current_node->key < b[dim] ) { /* split: a < key < b */
        right_path = current_node->right; /* both right */
        left_path  = current_node->left;    /* and left */
        break;

      } else if ( a[dim] == current_node->key ) { /* a = key < b */
        right_path = current_node->right; /* no left */
        break;

      } else { /*   current_node->key == b, so a < key = b */
        left_path  = current_node->left; /* no right */
        break;
      }
    }

    if ( left_path != NULL ) {

      /* now follow the path of the left endpoint a*/
      while ( left_path->right != NULL ) {

        if ( a[dim] < left_path->key ) {
          /* right node must be selected */
          new_list = find_points(
                       left_path->right->l_dim_tree, a, b, dim - 1);
          current_list = join_list( new_list, current_list);
          left_path = left_path->left;

        } else if ( a[dim] == left_path->key ) {
          new_list = find_points(
                       left_path->right->l_dim_tree, a, b, dim - 1);
          current_list = join_list( new_list, current_list);
          break; /* no further descent necessary */

        } else /* go right, no node selected */
        { left_path = left_path->right; }
      }

      /* left leaf needs to be selected if reached in descent*/
      if ( left_path->right == NULL && left_path->key == a[dim] ) {
        new_list = find_points(
                     left_path->l_dim_tree, a, b, dim - 1);
        current_list = join_list( new_list, current_list);
      }
    }  /* end left path */

    if ( right_path != NULL ) {

      /* and now follow the path of the right endpoint b */
      while ( right_path->right != NULL ) {

        if ( right_path->key < b[dim] ) {
          /* left node must be selected */
          new_list = find_points(
                       right_path->left->l_dim_tree, a, b, dim - 1);
          current_list = join_list( new_list, current_list);
          right_path = right_path->right;

        } else if ( right_path->key == b[dim]) {
          new_list = find_points(
                       right_path->left->l_dim_tree, a, b, dim - 1);
          current_list = join_list( new_list, current_list);
          break; /* no further descent necessary */

        } else /* go left, no node selected */
        { right_path = right_path->left; }
      }

      if ( right_path->right == NULL && right_path->key < b[dim] ) {
        new_list = find_points(
                     right_path->l_dim_tree, a, b, dim - 1);
        current_list = join_list( new_list, current_list);
      }
    }  /* end right path */
  }
  return ( current_list );
}

// _____________________________________________________________________________
//                                                                 Sample test

int
main()
{
  tree_node_t* or_tree;
  char nextop;
  int i = 0;
  p_list_node_t* pt_list, *pt_tmp;
  pt_list = NULL;
  printf("Please enter the points (2d)\n");

  while ( (nextop = getchar()) != 'q' ) {

    if ( nextop == 'i' ) {
      int a, b, c;
      scanf(" %d %d", &a, &b);
      pt_tmp = get_p_list_node();
      pt_tmp->point = (point_t*) malloc( sizeof( point_t ));
      pt_tmp->point->coordinate[0] = a;
      pt_tmp->point->coordinate[1] = b;
      pt_tmp->point->object = 42;
      pt_tmp->next = pt_list;
      pt_list = pt_tmp;
      i += 1;
    }
  }

  printf("stored %d points; they are\n", i);
  for ( pt_tmp = pt_list; pt_tmp != NULL; pt_tmp = pt_tmp->next )
  { printf("(%d,%d) ", pt_tmp->point->coordinate[0], pt_tmp->point->coordinate[1]); }

  printf("now building tree\n");
  or_tree = build_or_r_tree(pt_list, 1);
  printf("Made Tree\n");
  printf("Now entering query phase\n");

  while ( (nextop = getchar()) != 'q' ) {
    p_list_node_t* answer_list;

    if ( nextop == 'f' ) {
      int a[2], b[2];
      scanf(" %d %d %d %d", a, a + 1, b, b + 1);
      answer_list = find_points( or_tree, a, b, 1 );

      if ( answer_list == NULL )
      { printf("  no points found in the box %d %d %d %d\n", a[0], a[1], b[0], b[1] ); }

      else
      { printf("  the following points are in the query box: "); }

      while ( answer_list != NULL ) {
        printf(" (%d,%d) ", answer_list->point->coordinate[0], answer_list->point->coordinate[1]);
        answer_list = answer_list->next;
      }

      printf(" end\n");
    }
  }
  return (0);
}
