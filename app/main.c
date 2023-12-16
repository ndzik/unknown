#include "allocator.h"
#include "graph.h"
#include <stdio.h>

#define ARENA_SIZE 8192

int main() {
  // Allocate a simple stack allocator, efficient, easy and quick.
  char arena[ARENA_SIZE] = {0};
  StackAllocator strategy = new_stack_allocator(arena, ARENA_SIZE);

  struct Allocator allocator = {};
  allocator.strategy = &strategy;
  allocator.alloc = stack_alloc;
  allocator.free_all = stack_free;
  graph_set_allocator(allocator);

  Node *n1 = new_empty_node();
  if (n1 == NULL) {
    printf("error creating empty node\n");
    return 1;
  }
  Node *n2 = new_empty_node();
  if (n2 == NULL) {
    printf("error creating empty node\n");
    return 1;
  }
  Node *n3 = new_empty_node();
  if (n3 == NULL) {
    printf("error creating empty node\n");
    return 1;
  }
  Node *n4 = new_empty_node();
  if (n4 == NULL) {
    printf("error creating empty node\n");
    return 1;
  }
  GRAPH_CHECK_SUCCESS(new_node(NULL, 0, n1), "error creating new node");
  GRAPH_CHECK_SUCCESS(
      new_node(new_neighbors((const Node **)(Node *[]){n1}, 1), 1, n2),
      "error creating new node");
  GRAPH_CHECK_SUCCESS(
      new_node(new_neighbors((const Node **)(Node *[]){n1}, 1), 1, n3),
      "error creating new node");
  GRAPH_CHECK_SUCCESS(
      new_node(new_neighbors((const Node **)(Node *[]){n1, n2, n3}, 3), 3, n4),
      "error creating new node");

  printf("node %p\n", (void *)&n1);
  print_node(n1);
  printf("node %p\n", (void *)&n2);
  print_node(n2);
  printf("node %p\n", (void *)&n3);
  print_node(n3);

  printf("node %p\n", (void *)&n4);
  print_node(n4);
  return 0;
}
