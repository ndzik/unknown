#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node;

// Node based Graph, each node knows about its own connections.
typedef struct {
  // List of neighbors.
  struct Node **neighbors;
  int num_of_neighbors;
} Node;

Node *new_node(Node **neighbors, int num_of_neighbors) {
  // Allocated node on heap.
  Node *node = malloc(sizeof(Node));
  node->neighbors = (struct Node **)neighbors;
  node->num_of_neighbors = num_of_neighbors;

  if (neighbors == NULL || num_of_neighbors == 0) {
    return node;
  }

  // Iterate through all neighbor nodes and add this node to their list of
  // neighbors.
  for (int o = 0; o < num_of_neighbors; o++) {
    Node *neighbor = neighbors[o];
    // Allocate a new array which allows to store all neighbors.
    Node **new_neighbors =
        malloc((neighbor->num_of_neighbors + 1) * sizeof(Node));

    if (neighbor->num_of_neighbors > 0) {
      // Copy all old neighbors to new array and slot the new neighbor as in as
      // its last element.
      memcpy(new_neighbors, neighbor->neighbors,
             neighbor->num_of_neighbors * sizeof(Node));
    }

    // Set last neighbor to be the new allocated node.
    new_neighbors[neighbor->num_of_neighbors] = node;
    // Reset neighbors to new neighbors.
    neighbor->neighbors = (struct Node **)new_neighbors;
    // Increment the number of registered neighbors.
    neighbor->num_of_neighbors++;
  }

  return node;
}

void print_node(Node *node) {
  printf("node has %d neighbors\n", node->num_of_neighbors);
  if (node->num_of_neighbors > 0) {
    printf("\t%p -> [", (void *)node->neighbors);
    for (int o = 0; o < node->num_of_neighbors; o++) {
      printf(" %p", (void *)node->neighbors[o]);
    }
    printf(" ]\n");
  }
}
