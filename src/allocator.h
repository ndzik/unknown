#ifndef ALLOCATOR_H
#define ALLOCATOR_H

// Handles requests for memory allocations and frees.
typedef struct Allocator {
  // The implementation for this allocator.
  void *strategy;
  // Allocates a contiguous array of the given size in bytes.
  void *(*alloc)(struct Allocator *allocator, unsigned int sz_bytes);
  // Reallocates the data at the given pointer reserving the given amount of
  // bytes.
  void *(*realloc)(struct Allocator *allocator, void *address,
                   unsigned int sz_bytes);
  // Free reserved memory at the given address.
  void (*free)(struct Allocator *allocator, void *address);
  // Frees all reserved/used memory managed by this allocator.
  void (*free_all)(struct Allocator *allocator);
} Allocator;

typedef struct StackAllocator {
  unsigned int end;

  // The memory arena associated with this allocator. It has to point to a
  // static memory location during its management-cycle, otherwise the provided
  // pointers would not work.
  void *arena;

  unsigned int max_size;
} StackAllocator;

StackAllocator new_stack_allocator(void *arena, unsigned int arena_size);
void *stack_alloc(struct Allocator *allocator, unsigned int sz_bytes);
void stack_free(struct Allocator *allocator);

typedef struct HeapAllocator {
} HeapAllocator;

void *heap_alloc(struct Allocator *allocator, unsigned int sz_bytes);
void *heap_realloc(struct Allocator *allocator, void *address,
                   unsigned int sz_bytes);
void heap_free(struct Allocator *allocator, void *address);

#endif // ALLOCATOR_H
