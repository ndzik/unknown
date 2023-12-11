#ifndef BTREE_H
#define BTREE_H

#include "vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// BTree according to Donald Ervin Knuth:
// 1. Every node has at most m children
// 2. Every internal node has at least /m/2\ children
// 3. Root node has at least two children unless it is a leaf
// 4. All leaves appear on the same level
// 5. Non-leaf node with k children contains k-1 keys
//
// Internal node: Any node that is not a leaf or root node.
// Leaf node: Node without children.

// Each node has:
// * an array of references to child-trees
// * an array of keys
// * a parallel array of data-references for each key
typedef struct {
  Vector children;

  // Data and Keys are parallel arrays.
  Vector data;
  Vector keys;
} Node;

typedef struct {
  Node root;
  unsigned int max_num_childs;
} BTree;

typedef enum {
  BTREE_SUCCESS,
  BTREE_FAILURE,
  BTREE_REDUNDANT_INIT,
  BTREE_FULL,
} BTreeError;

int show_btree_error(BTreeError error_code, char *restrict target);
BTree *new_btree_root(unsigned int max_num_childs);

#define BTREE_CHECK_SUCCESS(error_code, error_message)                         \
  if (error_code != BTREE_SUCCESS) {                                           \
    char target[20];                                                           \
    show_btree_error(error_code, target);                                      \
    printf("%s: %s", target, error_message);                                   \
    return error_code;                                                         \
  }

#endif // BTREE_H
