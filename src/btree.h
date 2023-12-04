#pragma once

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
  struct Node **children;
  int num_of_children;

  // Data and Keys are parallel arrays.
  void **data;
  unsigned int *keys;
  unsigned int num_of_keys;
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
static BTreeError init_node(Node *node, unsigned int max_num_childs);
static BTreeError insert_node(Node *node, unsigned int t, unsigned int key,
                              void *value);
static BTreeError insert_key_value(BTree *tree, unsigned int key, void *value);
static BTreeError insert_key_sorted(Node *node, unsigned int key,
                                    unsigned int *tgt_idx);

#define BTREE_CHECK_SUCCESS(error_code, error_message)                         \
  if (error_code != BTREE_SUCCESS) {                                           \
    char target[20];                                                           \
    show_btree_error(error_code, target);                                      \
    printf("%s: %s", target, error_message);                                   \
    return error_code;                                                         \
  }
