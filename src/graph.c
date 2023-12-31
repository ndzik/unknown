#include "graph.h"
#include "allocator.h"

static HeapAllocator default_allocator = {};
static Allocator allocator = {
    .strategy = &default_allocator,
    .alloc = heap_alloc,
    .realloc = heap_realloc,
    .free = heap_free,
};

void graph_set_allocator(Allocator injected_alloc) {
  allocator = injected_alloc;
}

static GraphError add_node_to(Node *neighbor, Node *added_node);

// Allocates memory for a new node and returns its memory location.
Node *new_empty_node() {
  Node *node = allocator.alloc(&allocator, sizeof(Node));
  return node;
}

// Returns the memory address where the given array of neighbors are stored.
Node **new_neighbors(const Node **neighbors, int num_of_neighbors) {
  Node **moved_neighbors =
      allocator.alloc(&allocator, num_of_neighbors * sizeof(Node *));
  if (moved_neighbors == NULL) {
    return NULL;
  }
  for (int o = 0; o < num_of_neighbors; o++) {
    moved_neighbors[o] = (Node *)neighbors[o];
  }
  return moved_neighbors;
}

// This has multiple issues:
// 1. Memory responsibilities are not clear. The user of this function is able
//    to decide how to allocate the passed in `neighbors`.
//    The implementer of this library function would like to avoid unnecessary
//    copying of data, so just assigning the passed in neighbors is the easiest
//    approach. No one knows with confidence at any point in time whether a
//    node within a graph lives completely on the stack or the heap.
// 2. A result from the weird datastructure itself. Adding nodes effects each
//    related entity. Every node is supposed to know about its neighbors, so
//    adding a node requires updating the neighbor nodes. This can only be
//    consistently handled by reallocating a new array, which has to happen
//    on the heap.
// Result:
//    The library has to put constraints and expose a documentation together
//    with helper functions. This means `Node **neighbors` have to be created
//    only by means of the library API, which can force allocations on the heap.
//    This has the downside, that heap allocation will be done, where there is
//    no heap allocation necessary.
GraphError new_node(Node **neighbors, int num_of_neighbors, Node *node_result) {
  if (node_result == NULL) {
    return GRAPH_INVALID_MEMORY_RESULT;
  }
  node_result->neighbors = (struct Node **)neighbors;
  node_result->num_of_neighbors = num_of_neighbors;

  if (neighbors == NULL && num_of_neighbors == 0) {
    return GRAPH_SUCCESS;
  }

  if (neighbors == NULL && num_of_neighbors > 0) {
    return GRAPH_INVALID_ARG;
  }

  // Iterate through all neighbor nodes and add this node to their list of
  // neighbors.
  for (int o = 0; o < num_of_neighbors; o++) {
    GRAPH_CHECK_SUCCESS(add_node_to(neighbors[o], node_result),
                        "unable to add node as neighbor");
  }

  return GRAPH_SUCCESS;
}

void print_node(const Node *node) {
  printf("node has %d neighbors\n", node->num_of_neighbors);
  if (node->num_of_neighbors > 0) {
    printf("\t%p -> [", (void *)node->neighbors);
    for (int o = 0; o < node->num_of_neighbors; o++) {
      printf(" %p", (void *)node->neighbors[o]);
    }
    printf(" ]\n");
  }
}

static GraphError add_node_to(Node *neighbor, Node *added_node) {
  // Allocate a new array which allows to store all neighbors.
  Node **new_neighbors = allocator.alloc(
      &allocator, (neighbor->num_of_neighbors + 1) * sizeof(Node));

  if (new_neighbors == NULL) {
    return GRAPH_ALLOC_FAILED;
  }

  if (neighbor->num_of_neighbors > 0) {
    // Copy all old neighbors to new array and slot the new neighbor as in as
    // its last element.
    memcpy(new_neighbors, neighbor->neighbors,
           neighbor->num_of_neighbors * sizeof(Node));
  }

  // Set last neighbor to be the new allocated node.
  new_neighbors[neighbor->num_of_neighbors] = added_node;
  // Reset neighbors to new neighbors.
  neighbor->neighbors = (struct Node **)new_neighbors;
  // Increment the number of registered neighbors.
  neighbor->num_of_neighbors++;

  return GRAPH_SUCCESS;
}
