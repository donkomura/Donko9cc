#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#define STR(var) #var

enum {
  TK_NUM = 256, // numeric token
  TK_IDENT,    // operator token
  TK_EOF,       // end token
  TK_EQ,        // ==
  TK_NE,        // != 
  TK_LE,        // <=
  TK_GE,        // >=
  TK_LT,        // <
  TK_GT,        // >
  TK_RETURN,    // return
  TK_IF,        // if 
  TK_WHILE,     // while
  TK_ELSE,      // else 
  TK_FOR,       // for
};

enum {
  ND_NUM = 256, // node type of numeric
  ND_IDENT,    // node type of operator
  ND_EQ,        // ==
  ND_NE,        // != 
  ND_LE,        // <=
  ND_GE,        // >=
  ND_LT,        // <
  ND_GT,        // >
  ND_RETURN,    // return statement
  ND_IF,        // if statement
  ND_WHILE,     // while statement
  ND_FOR,       // for statement
  ND_BLOCK      // block
};

typedef struct {
  void **data;
  int capacity;  // buffer allocated area
  int len;       // elements count
} Vector;

Vector *new_vector(void); // create new vector
void vec_push(Vector *vec, void *elem); // push back in vector
void show_vec(Vector *vec, char type);

typedef struct {
  int ty;       // token type
  int val;      // Numeric token
  char *input;  // error point (for error message)
  char *name;   // variable name
} Token;

// tokenize input expression
void tokenize(char *p);
int is_alnum(char c);

typedef struct Node {
  int ty;           // node type from token type
  struct Node *lhs; // left hand side node
  struct Node *rhs; // right hand side node
  int val;          // the value of this node (ND_NUM)
  char *name;       // use in case ty == ND_INDNET

  // "if" ( cond ) then "else" els
  struct Node *cond;
  struct Node *then;
  struct Node *els;

  // "while" ( cond ) body
  struct Node *body;

  // "for" ( init; cond; inc ) body
  struct Node *init;
  struct Node *inc;

  // "{" block "}"
  struct Vector *block;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int ty);
Node *new_node_indent(char *name);
int consume(int ty);

// generate assembly functions
// details: syntax.txt
void program();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
void gen_lval(Node *node);
void gen(Node *node); // generate assembly

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

Map *new_map();
void map_set(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

// error functions
void error(char *fmt, ...); // error output
void error_at(char *loc, char *msg); // error output with error point in input expression

// test function
int expect(int line, int expected, int actual);
void runtest();
void test_vector();
void test_map();

Vector *tokens;
Vector *code;
Map *map;
int pos, code_pos, offset_count;
