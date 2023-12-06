#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>

StackAllocator new_stack_allocator(void *arena, unsigned int arena_size) {
  StackAllocator a = {};
  a.arena = arena;
  a.max_size = arena_size;
  return a;
}

void *stack_alloc(struct Allocator *allocator, unsigned int sz_bytes) {
  StackAllocator *sa = (StackAllocator *)allocator->strategy;
  printf("reserving %u bytes space\n", sz_bytes);
  if (sa->end + sz_bytes > sa->max_size) {
    printf("stack allocator overflow detected, aborting...");
    exit(1);
  }
  const unsigned int begin = sa->end;
  sa->end += sz_bytes;
  return sa->arena + begin;
}

void stack_free(struct Allocator *allocator) {
  StackAllocator *sa = (StackAllocator *)allocator->strategy;
  sa->end = 0;
}
