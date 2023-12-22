#include "vector.h"
#include <stdio.h>

typedef int (*TEST_CASE)(void);
typedef enum {
  FAILURE,
  SUCCESS,
} TEST_RESULT;

typedef struct TestCase {
  TEST_CASE test_fun;
  const char *name;
} TestCase;

typedef struct VectorTestStruct {
  unsigned int a;
  unsigned int b;
  double c;
  float d;
} VectorTestStruct;

#define ASSERT(actual, expected, message)                                      \
  if (actual != expected) {                                                    \
    printf(message "\n", actual, expected);                                    \
    return FAILURE;                                                            \
  }

void f(void *el, void *result) {
  unsigned int elem = *(unsigned int *)el;
  VectorTestStruct *vts = (VectorTestStruct *)result;
  vts->a = elem;
  vts->b = elem;
  vts->c = (double)elem;
  vts->d = (float)elem;
}

void x(void *el, void *result) {
  VectorTestStruct *vts = (VectorTestStruct *)el;
  vts->a += 10;
  *(unsigned int *)result = vts->a;
}

int vector_test() {
  VectorParams params = {
      .stride = sizeof(unsigned int),
  };
  unsigned int *my_vec = VEC(unsigned int, 512);
  ASSERT(v_length(my_vec), 0,
         "initialized vector with malformed length actual: %d expected: %d");
  ASSERT(v_capacity(my_vec), 512,
         "initialized vector should have correct capacity actual: %d expected: "
         "%d");

  unsigned int capacity = v_capacity(my_vec);
  for (unsigned int o = 0; o < capacity; ++o) {
    my_vec = v_append(my_vec, &o);
  }
  ASSERT(v_length(my_vec), 512,
         "vector should report correct length actual: %d expected %d");
  ASSERT(v_capacity(my_vec), 512,
         "vector should only resize (capacity) when overfilling actual: %d "
         "expected: %d");
  unsigned int length = v_length(my_vec);
  for (unsigned int o = 0; o < length; ++o) {
    ASSERT(my_vec[o], o,
           "vector entries should match expected actual: %d expected: %d");
  }

  unsigned int new_stride = sizeof(VectorTestStruct);
  VectorTestStruct *mapped_vec = V_MAP_VEC(my_vec, f, VectorTestStruct);
  ASSERT(v_stride(my_vec), params.stride,
         "vector stride after immutable map should not change actual: %d "
         "expected: %d");
  ASSERT(v_stride(mapped_vec), new_stride,
         "mapped vector should use stride for mapped elements actual: %d "
         "expected: %d");
  ASSERT(v_capacity(mapped_vec), capacity,
         "mapping full vector with bigger stride than original should not "
         "change its base capacity actual: %d expected: %d");
  for (unsigned int o = 0; o < length; ++o) {
    VectorTestStruct vts = mapped_vec[o];
    VectorTestStruct expected = {
        .a = o,
        .b = o,
        .c = (double)o,
        .d = (float)o,
    };
    ASSERT(vts.a, expected.a,
           "a of mapped vector should match actual: %d expected %d");
    ASSERT(vts.b, expected.b,
           "b of mapped vector should match actual: %d expected %d");
    ASSERT(vts.c, expected.c,
           "c of mapped vector should match actual: %lf expected %lf");
    ASSERT(vts.d, expected.d,
           "d of mapped vector should match actual: %f expected %f");
  }

  VectorTestStruct *mutably_mapped = V_MAP_VEC_M(my_vec, f, VectorTestStruct);
  for (unsigned int o = 0; o < length; ++o) {
    VectorTestStruct expected = mapped_vec[o];
    VectorTestStruct actual = mutably_mapped[o];
    ASSERT(actual.a, expected.a,
           "a of mapped vector should match actual: %d expected %d");
    ASSERT(actual.b, expected.b,
           "b of mapped vector should match actual: %d expected %d");
    ASSERT(actual.c, expected.c,
           "c of mapped vector should match actual: %.32lf expected %.32lf");
    ASSERT(actual.d, expected.d,
           "d of mapped vector should match actual: %f  expected %f ");
  }

  my_vec = v_map_m(mutably_mapped, x, sizeof(unsigned int));
  for (unsigned int o = 0; o < length; ++o) {
    unsigned int expected = o + 10;
    unsigned int actual = my_vec[o];
    ASSERT(actual, expected,
           "a of mapped vector should match actual: %d expected %d");
  }

  return SUCCESS;
}

int main() {
  TestCase test_cases[] = {
      {.test_fun = vector_test, .name = "VECTOR_TEST"},
      {0}, // Sentinel value, always last element.
  };
  TestCase test_case = test_cases[0];
  printf("unknown-test-running...\n");
  for (int o = 1; test_case.test_fun != NULL; ++o) {
    if (!test_case.test_fun()) {
      printf("%s failed!\n", test_case.name);
    } else {
      printf("%s success!\n", test_case.name);
    }
    test_case = test_cases[o];
  }
  printf("unknown-test-finished...\n");
  return 0;
}
