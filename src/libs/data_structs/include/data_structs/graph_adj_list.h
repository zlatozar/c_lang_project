/**
 * @file    graph_adj_list.h
 * @brief   Graph structure represented using adjacency list.
 */
#if !defined(DATA_STRUCTS_GRAPH_ADJ_LIST_H)
#define DATA_STRUCTS_GRAPH_ADJ_LIST_H

#include <stddef.h>      /* size_t */
#include "graph_rep.h"
#include "list.h"
#include "lang/extend.h"

/* Contains edge number and list to its adjacent vertices. */
typedef struct graph_descriptor graph_t;

typedef graph_t* Graph_T;

/* List node now contains edge information. */
typedef node_t edge_t;

/**
 * @brief    Initialize a graph structure.
 */
extern Graph_T Graph_new(size_t number_of_vertices, graph_type_et type);

/**
 * Add an edge between `vertex1` and `vertex2` with given weight. If the graph
 * is undirected, place an identical edge between `vertex2` and
 * `vertex1`(mirror).
 */
extern void Graph_add_edge(Graph_T graph, vertex vertex1, vertex vertex2,
                           int16_t weight);

/**
 * Return TRUE if there is an edge from vertexl to vertex2.
 */
extern bool Graph_is_adjacent(Graph_T graph, vertex vertex1, vertex vertex2);

/**
 * Return the number of vertices in `p_vertex_cnt__` and the number of
 * edges in `p_edge_cnt__`.
 */
extern void Graph_size(Graph_T graph, size_t* p_vertex_cnt__, size_t* p_edge_cnt__);

/**
 * Return all the edges out of `vertex_number` in turn. To start
 * `p_last_return__` should be NULL. In subsequent calls, it should be what
 * was returned in the previous call. When there are no more edges, NULL is
 * returned.
 */
extern edge_t* Graph_edge_iterator(Graph_T graph, vertex vertex_number,
                                   edge_t* p_last_return__);

/**
 * Delete an entire graph by destroying the lists and then
 * deallocating edges list and the graph header.
 */
void Graph_destroy(Graph_T* p_graph, free_data_FN free_data_fn);

void Graph_free(Graph_T* p_graph);

#endif  /* DATA_STRUCTS_GRAPH_ADJ_LIST_H */
