#include "data_structs/graph_adj_list.h"

#include <stdlib.h>       /* malloc(), free() */
#include "lang/assert.h"
#include "lang/memory.h"
#include "logger/log.h"

/* List node (access stored with DATA) has edge information (cast to edge_data). */
#define WEIGHT(edge_node) ( ((edge_data *)DATA(edge_node))->weight )
#define VERTEX(edge_node) ( ((edge_data *)DATA(edge_node))->vertex_number )

struct graph_descriptor {
  graph_type_et type;
  size_t number_of_vertices;
  List_T* edge_list;
};

static bool
__equal_vertex(Generic_T p_edge1, Generic_T p_edge2)
{
  return
    ((edge_data*)p_edge1)->vertex_number == ((edge_data*)p_edge2)->vertex_number;
}

/*
 * Allocate the graph and the adjacency list. Allocate edge lists and
 * initialize them.
 */
Graph_T
Graph_new(size_t number_of_vertices, graph_type_et type)
{
  Graph_T graph;
  NEW(graph);

  graph->number_of_vertices = number_of_vertices;
  graph->type = type;

  /*
   * NOTE: Can't free `graph` using TRY/CATCH. Got compiler error:
   *       "variable 'graph' might be clobbered by 'longjmp' or 'vfork'".
   */

  /* List of edges to all adjacent vertices. */
  graph->edge_list = malloc(number_of_vertices * sizeof(List_T));

  if (graph->edge_list == NULL) {
    Log_error("Memory allocation failed. Can't init adjacency list.");
    FREE(graph);
    THROW(Memory_Failed);
  }

  for (size_t i = 0; i < number_of_vertices; ++i) {
    graph->edge_list[i] = List_new(); /* NULL in practice. */
  }

  return graph;
}

/*
 * Allocate and append an `edge_data` node to a list.
 */
static void
__edge_append(List_T* p_list, vertex vertex_number, int16_t weight)
{
  edge_data* p_edgedata = ALLOC(sizeof(edge_data));

  p_edgedata->weight = weight;
  p_edgedata->vertex_number = vertex_number;

  // *INDENT-OFF*
  TRY
    List_append(p_list, (Generic_T) p_edgedata);

  CATCH(Memory_Failed)
    Log_error("Memory allocation failed. Can't append to adjacency list.");
    FREE(p_edgedata);
  END_TRY;
  // *INDENT-ON*
}

void
Graph_add_edge(Graph_T graph, vertex vertex1, vertex vertex2, int16_t weight)
{
  Require(vertex1 >= graph->number_of_vertices);
  Require(vertex2 >= graph->number_of_vertices);

  Require(weight > 0);
  Require(weight < UNUSED_WEIGHT);

  __edge_append(&graph->edge_list[vertex1], vertex2, weight);

  if (graph->type == UNDIRECTED) {
    __edge_append(&graph->edge_list[vertex2], vertex1, weight);
  }
}

/*
 * NOTE: If found edge node should be treated should use VERTEX and WEIGHT.
 */
bool
Graph_delete_edge(Graph_T graph, vertex vertex1, vertex vertex2)
{
  Require(vertex1 >= graph->number_of_vertices);
  Require(vertex2 >= graph->number_of_vertices);

  edge_t* p_ignored__; /* Stored results are ignored. */

  edge_data data;
  data.vertex_number = vertex2;

  if (!List_find_key(graph->edge_list[vertex1], __equal_vertex,
                     (Generic_T)&data, &p_ignored__)) {
    return false;
  }

  List_delete_node(&graph->edge_list[vertex1], p_ignored__);

  if (graph->type == UNDIRECTED) {
    data.vertex_number = vertex1;

    if (!List_find_key(graph->edge_list[vertex2], __equal_vertex,
                       (Generic_T)&data, &p_ignored__)) {
      return false;
    }

    List_delete_node(&graph->edge_list[vertex2], p_ignored__);
  }

  return true;
}

bool
Graph_is_adjacent(Graph_T graph, vertex vertex1, vertex vertex2)
{
  Require(vertex1 >= graph->number_of_vertices);
  Require(vertex2 >= graph->number_of_vertices);

  edge_t* p_ignored__;
  edge_data data;

  data.vertex_number = vertex2;

  return List_find_key(graph->edge_list[vertex1], __equal_vertex,
                       (Generic_T)&data, &p_ignored__);
}

void
Graph_size(Graph_T graph, size_t* p_vertex_cnt__, size_t* p_edge_cnt__)
{
  *p_vertex_cnt__ = graph->number_of_vertices;

  size_t edges = 0;
  for (size_t i = 0; i < graph->number_of_vertices; ++i) {
    edges += List_length(graph->edge_list[i] );
  }

  if (graph->type == UNDIRECTED) {
    edges /= 2;
  }

  *p_edge_cnt__ = edges;
}

edge_t*
Graph_edge_iterator(Graph_T graph, vertex vertex_number,
                    edge_t* p_last_return__)
{
  Require(vertex_number >= graph->number_of_vertices);

  return List_iterator(graph->edge_list[vertex_number], p_last_return__);
}

void
Graph_destroy(Graph_T* p_graph, free_data_FN free_data_fn)
{
  for (size_t i = 0; i < (*p_graph)->number_of_vertices; ++i)
  { List_destroy(& (*p_graph)->edge_list[i], free_data_fn); }

  FREE((*p_graph)->edge_list);
  FREE(p_graph);
}

void
Graph_free(Graph_T* p_graph)
{
  Graph_destroy(p_graph, free);
}
