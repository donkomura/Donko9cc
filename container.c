#include "9cc.h"

char *user_input;

// test code for component functions --------------------------------
int expect(int line, int expected, int actual) {
  if (expected == actual)
    return 0;
  fprintf(stderr, "%d: %d expected, but god %d\n",
      line, expected, actual);
  exit(1);
}

void test_vector() {
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

void test_map() {
  Map *map = new_map();
  expect(__LINE__, 0, (long)map_get(map, "hoge"));

  map_set(map, "hoge", (void *)2);
  expect(__LINE__, 2, (long)map_get(map, "hoge"));

  map_set(map, "pue", (void *)4);
  expect(__LINE__, 4, (long)map_get(map, "pue"));

  map_set(map, "hoge", (void *)8);
  expect(__LINE__, 8, (long)map_get(map, "hoge"));
  return;
}

void runtest() {
  test_vector();
  test_map();
}
// ------------------------------------------------------------------

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

void vec_pushi(Vector *v, int val) {
  vec_push(v, (void *)(intptr_t)val);
}
// show vector elements
void show_vec(Vector *vec, char type) {
  for (int i = 0; i < vec->len; i++) {
    if (type == 's') {
      fprintf(stdout, "%s", (char *)vec->data[i]);
    } else if (type == 'c') {
    } else if (type == 'd') {
    }
  }
  printf("\n");
}

// verify a charactar: number, alphabet, '_'
int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

// create new map
Map *new_map() {
  Map *map = malloc(sizeof(Map));
  map->keys = new_vector();
  map->vals = new_vector();
  return map;
}

// set key-value
void map_set(Map *map, char *key, void *val) {
  vec_push(map->keys, key);
  vec_push(map->vals, val);
  return;
}

// get value
void *map_get(Map *map, char *key) {
  for (int i = map->keys->len - 1; i >= 0; i--) {
    if (strcmp(map->keys->data[i], key) == 0)
      return map->vals->data[i];
  }
  return NULL;
}
