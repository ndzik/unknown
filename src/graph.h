#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node based Graph, each node knows about its own connections.
typedef struct {
  // List of neighbors.
  struct Node **neighbors;
  int num_of_neighbors;
} Node;

typedef enum {
  GRAPH_SUCCESS,
  GRAPH_INVALID_ARG,
  GRAPH_INVALID_MEMORY_RESULT,
  GRAPH_MALLOC_FAILED,
} GraphError;

// Macro to check for `GRAPH_SUCCESS`, printing the given error message and
// returning with the given error code.
#define GRAPH_CHECK_SUCCESS(error_code, error_message)                         \
  if (error_code != GRAPH_SUCCESS) {                                           \
    printf("%s\n", error_message);                                             \
    return error_code;                                                         \
  }

GraphError new_node(Node **neighbors, int num_of_neighbors, Node *node_result);
Node *new_empty_node();
Node **new_neighbors(const Node **neighbors, int num_of_neighbors);
void print_node(const Node *node);
