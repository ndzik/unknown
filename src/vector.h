#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

typedef void *Vector;
typedef void(MapFunction)(void *elem, void *result);

typedef struct VectorParams {
  unsigned int length;
  unsigned int capacity;
  unsigned int stride;
} VectorParams;

Vector new_vector(VectorParams params);
Vector v_append(Vector vec, void *value);
unsigned int v_base_offset(Vector vec);
unsigned int v_length(Vector vec);
void *v_at(Vector vec, unsigned int offset);
void v_inc_length(Vector vec);
void v_dec_length(Vector vec);
void v_set_length(Vector vec, unsigned int length);
unsigned int v_capacity(Vector vec);
void v_insert_at(Vector vec, unsigned int offset, void *value);
void v_set_at(Vector vec, unsigned int offset, void *value);
void v_set_capacity(Vector vec, unsigned int capacity);
unsigned int v_stride(Vector vec);
void v_free(Vector vec);
Vector v_map(Vector vec, MapFunction fun, unsigned int new_stride);
Vector v_map_m(Vector vec, MapFunction fun, unsigned int new_stride);
VectorParams *v_params(Vector vec);

#define VEC(type_var, len)                                                     \
  ({                                                                           \
    VectorParams params = {                                                    \
        .stride = sizeof(type_var),                                            \
        .capacity = len,                                                       \
    };                                                                         \
    (type_var *)new_vector(params);                                            \
  })

#define V_MAP_VEC(vec, fun, type_var)                                          \
  ({ (type_var *)v_map(vec, fun, sizeof(type_var)); })

#define V_MAP_VEC_M(vec, fun, type_var)                                        \
  ({ (type_var *)v_map_m(vec, fun, sizeof(type_var)); })

#endif // VECTOR_H
