#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

enum {
  TK_NUM = 256, // numeric token
  TK_INDENT,    // operator token
  TK_EOF,       // end token
  TK_EQ,        // ==
  TK_NE,        // != 
  TK_LE,        // <=
  TK_GE,        // >=
  TK_LT,        // <
  TK_GT         // >
};

enum {
  ND_NUM = 256, // node type of numeric
  ND_INDENT,    // node type of operator
  ND_EQ,        // ==
  ND_NE,        // != 
  ND_LE,        // <=
  ND_GE,        // >=
  ND_LT,        // <
  ND_GT         // >
};

typedef struct {
  int ty;       // token type
  int val;      // ty == ND_NUM
  char *input;  // error point (for error message)
} Token;

// tokenize input expression
void tokenize(char *p);

typedef struct Node {
  int ty;           // node type from token type
  struct Node *lhs; // left hand side node
  struct Node *rhs; // right hand side node
  int val;          // the value of this node (ND_NUM)
  char name;        // use in case ty == ND_INDNET
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int ty);
Node *new_node_indent(int ty);
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
  void **data;
  int capacity;  // buffer allocated area
  int len;       // elements count
} Vector;

Vector *new_vector(); // create new vector
void vec_push(Vector *vec, void *elem); // push back in vector

// error functions
void error(char *fmt, ...); // error output
void error_at(char *loc, char *msg); // error output with error point in input expression

// test function
int expect(int line, int expected, int actual);
void runtest();

Vector *tokens;
Vector *code;
int pos, code_pos;
