#include "btree.h"
#include "allocator.h"

static HeapAllocator default_allocator = {};
static Allocator allocator = {
    .strategy = &default_allocator,
    .alloc = heap_alloc,
    .realloc = heap_realloc,
    .free = heap_free,
};

static BTreeError init_node(Node *node, unsigned int max_num_childs);
static BTreeError insert_into_node(Node *node, unsigned int t, unsigned int key,
                                   void *value);
static BTreeError insert_key_value(BTree *tree, unsigned int key, void *value);
static BTreeError insert_key_sorted(Node *node, unsigned int key,
                                    unsigned int *tgt_idx);
static BTreeError insert_value_at(Node *node, void *value,
                                  unsigned int tgt_idx);

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
  BTree *btree = allocator.alloc(&allocator, sizeof(BTree));
  btree->max_num_childs = max_num_childs;
  // Initialize Node, already allocated because it is not a pointer member.
  btree->root.children = NULL;
  btree->root.data = NULL;
  btree->root.keys = NULL;
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
    return insert_into_node(&tree->root, tree->max_num_childs, key, value);
  }

  return BTREE_SUCCESS;
}

// Initializes the given nodes arrays, makes checks to not overwrite already
// existing arrays.
static BTreeError init_node(Node *node, unsigned int max_num_childs) {
  if (node->children != NULL || node->keys != NULL || node->data != NULL) {
    return BTREE_REDUNDANT_INIT;
  }

  VectorParams children_params = {
      .stride = sizeof(Node *),
      .capacity = max_num_childs,
  };
  VectorParams keys_params = {
      .stride = sizeof(unsigned int),
      .capacity = max_num_childs - 1,
  };
  VectorParams data_params = {
      .stride = sizeof(void *),
      .capacity = max_num_childs - 1,
  };
  node->children = new_vector(children_params);
  node->keys = new_vector(keys_params);
  node->data = new_vector(data_params);
  return BTREE_SUCCESS;
}

// The node is expected to be fully initialized and a leaf.
static BTreeError insert_into_node(Node *node, unsigned int t, unsigned int key,
                                   void *value) {
  unsigned int tgt_idx;
  BTREE_CHECK_SUCCESS(insert_key_sorted(node, key, &tgt_idx), "inserting key");

  BTREE_CHECK_SUCCESS(insert_value_at(node, value, tgt_idx), "inserting value");

  return BTREE_SUCCESS;
}

static BTreeError insert_value_at(Node *node, void *value,
                                  unsigned int tgt_idx) {
  v_insert_at(node->data, tgt_idx, value);
  return BTREE_SUCCESS;
}

static BTreeError insert_key_sorted(Node *node, unsigned int key,
                                    unsigned int *tgt_idx) {
  int o = 0;
  for (; o < v_length(node->keys); ++o) {
    if (*(unsigned int *)v_at(node->keys, o) >= key) {
      // We found the offset into the keys array that has a value greater than
      // our to be inserted key. Our key should be stored at its current
      // position and everything afterwards moved to the right.
      break;
    }
    // We do not insert in-place. Keep data coherent.
  }

  // Insertion fits, we modify the array in-place, starting from the back. This
  // avoids unnecessary memory allocation to store a tmp var.
  v_insert_at(node->keys, o, &key);

  // Insert our key, `o` now points to a free slot.
  v_set_at(node->keys, o, &key);
  *tgt_idx = o;
  return BTREE_SUCCESS;
}
