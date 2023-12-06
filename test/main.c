#include <stdio.h>

typedef int (*TEST_CASE)(void);
typedef enum {
  FAILURE,
  SUCCESS,
} TEST_RESULT;

int some_test(void) {
  printf("hello_from_test\n");
  return SUCCESS;
}

int main() {
  TEST_CASE test_cases[] = {
      some_test,
      NULL, // Sentinel value, always last element.
  };
  TEST_CASE test_case = test_cases[0];
  printf("unknown-test-running...\n");
  for (int o = 1; test_case != NULL; ++o) {
    if (test_case()) {
      printf("test-case true\n");
    }
    test_case = test_cases[o];
  }
  printf("unknown-test-finished...\n");
  return 0;
}
