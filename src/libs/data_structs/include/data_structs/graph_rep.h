/**
 * @file    graph_rep.h
 * @brief   Graph structure common structures.
 */
#if !defined(DATA_STRUCTS_GRAPH_REP_H)
#define DATA_STRUCTS_GRAPH_REP_H

#include <stdint.h>  /* INT16_MAX */

typedef size_t vertex;

/* Will be stored in graph `edge_list`. */
typedef struct {
  int16_t weight;
  vertex vertex_number;
} edge_data;

/* To indicate not reachable. */
#define UNUSED_WEIGHT (INT16_MAX)

typedef enum { DIRECTED, UNDIRECTED } graph_type_et;

#endif  /* DATA_STRUCTS_GRAPH_REP_H */
