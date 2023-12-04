#include "btree.h"

int show_btree_error(BTreeError error_code, char *restrict target) {
  switch (error_code) {
  case BTREE_SUCCESS:
    return sprintf(target, "SUCCESS");
  case BTREE_REDUNDANT_INIT:
    return sprintf(target, "REDUNDANT_INIT");
  case BTREE_FAILURE:
    return sprintf(target, "FAILURE");
  case BTREE_FULL:
    return sprintf(target, "FULL");
  }
}

BTree *new_btree_root(unsigned int max_num_childs) {
  BTree *btree = malloc(sizeof(BTree));
  btree->max_num_childs = max_num_childs;
  // Initialize Node, already allocated because it is not a pointer member.
  btree->root.children = NULL;
  btree->root.num_of_children = 0;
  btree->root.data = NULL;
  btree->root.keys = NULL;
  btree->root.num_of_keys = 0;
  return btree;
}


// Inserts arbitrary data with the given key into the given tree. Adheres to
// the balancing rules using tree->max_num_childs;
BTreeError insert_key_value(BTree *tree, unsigned int key, void *value) {
  // Check if root-node has space.
  if (tree->root.children == NULL) {
    // First ever entry, initialize node and insert.
    BTREE_CHECK_SUCCESS(init_node(&tree->root, tree->max_num_childs),
                        "initialising root node");
    return insert_node(&tree->root, tree->max_num_childs, key, value);
  }

  return BTREE_SUCCESS;
}

// Initializes the given nodes arrays, makes checks to not overwrite already
// existing arrays.
static BTreeError init_node(Node *node, unsigned int max_num_childs) {
  if (node->children != NULL || node->keys != NULL || node->data != NULL) {
    return BTREE_REDUNDANT_INIT;
  }

  node->children = malloc(max_num_childs * sizeof(Node *));
  node->keys = malloc((max_num_childs - 1) * sizeof(unsigned int));
  node->data = malloc((max_num_childs - 1) * sizeof(void *));
  return BTREE_SUCCESS;
}

// The node is expected to be fully initialized and a leaf.
static BTreeError insert_node(Node *node, unsigned int t, unsigned int key,
                              void *value) {
  if (node->num_of_children == (t - 1) || node->num_of_keys == t / 2) {
    return BTREE_FAILURE;
  }
  unsigned int tgt_idx;
  BTREE_CHECK_SUCCESS(insert_key_sorted(node, key, &tgt_idx), "inserting key");

  return BTREE_SUCCESS;
}

static BTreeError insert_key_sorted(Node *node, unsigned int key,
                                    unsigned int *tgt_idx) {
  int o = 0;
  for (; o < node->num_of_keys; ++o) {
    if (node->keys[o] >= key) {
      // We found the offset into the keys array that has a value greater than
      // our to be inserted key. Our key should be stored at its current
      // position and everything afterwards moved to the right.
      break;
    }
    // We do not insert in-place. Keep data coherent.
  }

  // Insertion fits, we modify the array in-place, starting from the back. This
  // avoids unnecessary memory allocation to store a tmp var.
  for (int k = node->num_of_keys; k > o; ++k) {
    node->keys[k] = node->keys[k - 1];
  }

  // Insert our key, `o` now points to a free slot.
  node->num_of_keys++;
  node->keys[o] = key;
  *tgt_idx = o;
  return BTREE_SUCCESS;
}