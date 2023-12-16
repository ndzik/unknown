#include "vector.h"
#include "allocator.h"

static HeapAllocator default_allocator = {};
static Allocator allocator = {
    .strategy = &default_allocator,
    .alloc = heap_alloc,
    .realloc = heap_realloc,
    .free = heap_free,
};

static Vector v_map_from_back(Vector vec, MapFunction fun, unsigned int stride);
static Vector v_map_from_front(Vector vec, MapFunction fun,
                               unsigned int stride);

Vector new_vector(VectorParams params) {
  // Defaults.
  unsigned int length = 0;
  unsigned int capacity = 512;

  if (params.length != 0)
    length = params.length;
  if (params.capacity != 0)
    capacity = params.capacity;

  // TODO: Better signal required.
  if (params.stride == 0)
    exit(420);
  unsigned int stride = params.stride;

  Vector vec =
      allocator.alloc(&allocator, stride * capacity + sizeof(VectorParams));
  ((unsigned int *)vec)[0] = length;
  ((unsigned int *)vec)[1] = capacity;
  ((unsigned int *)vec)[2] = stride;

  return vec;
}

Vector v_increase_size(Vector vec) {
  unsigned int capacity = v_capacity(vec);
  unsigned int stride = v_stride(vec);
  v_set_capacity(vec, capacity * 2);
  return allocator.realloc(&allocator, vec, 2 * capacity * stride);
}

Vector v_append(Vector vec, void *value) {
  unsigned int offset = base_offset(vec);
  VectorParams params = v_params(vec);

  if (params.length == params.capacity) {
    vec = v_increase_size(vec);
  }

  memcpy(vec + offset + params.stride * params.length, value, params.stride);
  v_inc_length(vec);
  return vec;
}

void v_drop(Vector vec, void *result) {
  unsigned int offset = base_offset(vec);
  VectorParams params = v_params(vec);
  if (params.length == 0) {
    result = NULL;
    return;
  }
  v_dec_length(vec);
  memcpy(result, vec + offset + params.stride * params.length, params.stride);
}

Vector v_map(Vector vec, MapFunction fun, unsigned int stride) {
  VectorParams params = {
      .length = 0,
      .capacity = v_capacity(vec),
      .stride = stride,
  };
  Vector mapped_vec = new_vector(params);
  unsigned int length = v_length(vec);
  char buf[stride];
  for (int o = 0; o < length; ++o) {
    fun(v_at(vec, o), buf);
    mapped_vec = v_append(mapped_vec, buf);
  }
  return mapped_vec;
}

Vector v_map_m(Vector vec, MapFunction fun, unsigned int new_stride) {
  unsigned int length = v_length(vec);
  unsigned int old_byte_length = length * v_stride(vec);
  unsigned int new_byte_length = length * new_stride;
  if (new_byte_length > old_byte_length) {
    // Make sure we always have enough memory available to fit all elements
    // when a vector is reaching full capacity.
    vec = allocator.realloc(&allocator, vec, v_capacity(vec) * new_stride);
  }
  if (v_stride(vec) < new_stride) {
    vec = v_map_from_back(vec, fun, new_stride);
  } else {
    vec = v_map_from_front(vec, fun, new_stride);
  }
  ((VectorParams *)vec)->stride = new_stride;
  return vec;
}

static Vector v_map_from_back(Vector vec, MapFunction fun,
                              unsigned int stride) {
  unsigned int length = v_length(vec);
  unsigned int offset = base_offset(vec);
  for (int o = (length - 1); o >= 0; --o) {
    fun(v_at(vec, o), vec + offset + o * stride);
  }
  return vec;
}

static Vector v_map_from_front(Vector vec, MapFunction fun,
                               unsigned int stride) {
  unsigned int offset = base_offset(vec);
  unsigned int length = v_length(vec);
  for (int o = 0; o < length; ++o) {
    fun(v_at(vec, o), vec + offset + o * stride);
  }
  return vec;
}

void *v_at(Vector vec, unsigned int offset) {
  if (offset >= v_length(vec)) {
    // TODO: Add logging.
    exit(-1);
  }

  return vec + base_offset(vec) + v_stride(vec) * offset;
}

void v_insert_at(Vector vec, unsigned int offset, void *value) {
  for (int k = v_length(vec); k > offset; --k) {
    v_set_at(vec, k, v_at(vec, k - 1));
  }
  v_set_at(vec, offset, value);
  v_inc_length(vec);
}

void v_set_at(Vector vec, unsigned int offset, void *value) {
  unsigned int b_offset = base_offset(vec);
  unsigned int stride = v_stride(vec);
  memcpy(vec + b_offset + stride * offset, value, stride);
}

unsigned int base_offset(Vector vec) { return sizeof(VectorParams); }
unsigned int v_length(Vector vec) { return ((VectorParams *)vec)->length; }
void v_inc_length(Vector vec) { ((VectorParams *)vec)->length++; }
void v_dec_length(Vector vec) { ((VectorParams *)vec)->length--; }
void v_set_length(Vector vec, unsigned int length) {
  ((VectorParams *)vec)->length = length;
}
unsigned int v_capacity(Vector vec) { return ((VectorParams *)vec)->capacity; }
void v_set_capacity(Vector vec, unsigned int capacity) {
  ((VectorParams *)vec)->capacity = capacity;
}
unsigned int v_stride(Vector vec) { return ((VectorParams *)vec)->stride; }
VectorParams v_params(Vector vec) { return *((VectorParams *)vec); }
