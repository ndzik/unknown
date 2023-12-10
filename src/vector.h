#pragma once

#include <stdlib.h>
#include <string.h>

typedef void *Vector;
typedef void *(MapFunction)(void *elem);

unsigned int base_offset(Vector vec);
unsigned int v_length(Vector vec);
void *v_at(Vector vec, unsigned int offset);
void v_inc_length(Vector vec);
void v_dec_length(Vector vec);
void v_set_length(Vector vec, unsigned int length);
unsigned int v_capacity(Vector vec);
void v_set_capacity(Vector vec, unsigned int capacity);
unsigned int v_stride(Vector vec);
Vector v_map(Vector vec, MapFunction fun, unsigned int stride);
static Vector v_map_from_back(Vector vec, MapFunction fun, unsigned int stride);
static Vector v_map_from_front(Vector vec, MapFunction fun,
                               unsigned int stride);

typedef struct VectorParams {
  unsigned int length;
  unsigned int capacity;
  unsigned int stride;
} VectorParams;

VectorParams v_params(Vector vec);

Vector make_vector(VectorParams params) {
  // Defaults.
  unsigned int length = 0;
  unsigned int capacity = 512;

  if (params.length != 0)
    length = params.length;
  if (params.capacity != 0)
    capacity = params.capacity;

  // Required.
  if (params.stride == 0)
    exit(420);
  unsigned int stride = params.stride;

  Vector vec = malloc(stride * capacity + sizeof(VectorParams));
  ((unsigned int *)vec)[0] = length;
  ((unsigned int *)vec)[1] = capacity;
  ((unsigned int *)vec)[2] = stride;

  return vec;
}

Vector v_resize(Vector vec) {
  unsigned int capacity = v_capacity(vec);
  v_set_capacity(vec, capacity * 2);
  return realloc(vec, 2 * capacity);
}

Vector v_append(Vector vec, void *value) {
  unsigned int offset = base_offset(vec);
  VectorParams params = v_params(vec);

  if (params.length == params.capacity) {
    vec = v_resize(vec);
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
      .length = v_length(vec),
      .capacity = v_capacity(vec),
      .stride = stride,
  };
  Vector mapped_vec = make_vector(params);
  for (int o = 0; o < params.length; ++o) {
    mapped_vec = v_append(mapped_vec, fun(v_at(vec, o)));
  }
  return mapped_vec;
}

Vector v_map_m(Vector vec, MapFunction fun, unsigned int new_stride) {
  unsigned int new_length = new_stride * v_length(vec);
  if (new_length > v_capacity(vec)) {
    vec = v_resize(vec);
  }
  if (v_stride(vec) < new_stride) {
    return v_map_from_back(vec, fun, new_stride);
  }
  return v_map_from_front(vec, fun, new_stride);
}

static Vector v_map_from_back(Vector vec, MapFunction fun,
                              unsigned int stride) {
  unsigned int length = v_length(vec);
  unsigned int offset = base_offset(vec);
  for (int o = (length - 1); o >= 0; --o) {
    // TODO: This does not work since we might catch up with the real data
    // while iterating.
    memcpy(vec + offset + o * stride, fun(v_at(vec, o)), stride);
  }
  ((VectorParams *)vec)->stride = stride;
  return vec;
}

static Vector v_map_from_front(Vector vec, MapFunction fun,
                               unsigned int stride) {
  unsigned int offset = base_offset(vec);
  unsigned int length = v_length(vec);
  for (int o = 0; o < length; ++o) {
    memcpy(vec + offset + o * stride, fun(v_at(vec, o)), stride);
  }
  ((VectorParams *)vec)->stride = stride;
  return vec;
}

void *v_at(Vector vec, unsigned int offset) {
  if (offset >= v_length(vec)) {
    // TODO: Add logging.
    exit(-1);
  }

  return vec + base_offset(vec) + v_stride(vec) * offset;
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