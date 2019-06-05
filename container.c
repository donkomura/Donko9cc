#include "9cc.h"

char *user_input;

// test code for Vector --------------------------------
int expect(int line, int expected, int actual) {
  if (expected == actual) 
    return;
  fprintf(stderr, "%d: %d expected, but god %d\n", 
      line, expected, actual);
  exit(1);
}

void runtest() {
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for (int i = 0; i < 100; i++) 
    vec_push(vec, (void *)i);
  
  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0,   (long)vec->data[0]);
  expect(__LINE__, 50,  (long)vec->data[50]);
  expect(__LINE__, 99,  (long)vec->data[99]);

  printf("OK\n");
}
// -----------------------------------------------------

// arert functions
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// pointing error
void error_at(char *loc, char *msg) {
  int posit = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", posit, "");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// create new vector 
Vector *new_vector() {
  Vector *vec = malloc(sizeof(Vector));
  vec->data = malloc(sizeof(void *) * 16);
  vec->capacity = 16;
  vec->len = 0;
  return vec;
}

// push back vector element
void vec_push(Vector *vec, void *elem) {
  if (vec->capacity == vec->len) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
  }
  vec->data[vec->len++] = elem;
}

// verify a charactar: number, alphabet, '_'
int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}
