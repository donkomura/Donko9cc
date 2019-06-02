#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

enum {
  ND_NUM = 256, // numeric
  ND_EQ, // ==
  ND_NE, // != 
  ND_LE, // <=
  ND_GE, // >=
  ND_LT, // <
  ND_GT  // >
};

typedef struct {
  int ty;       // token type
  int val;      // ty == ND_NUM
  char *input;  // error point (for error message)
} Token;

typedef struct Node {
  int ty;           // node type from token type
  struct Node *lhs; // left hand side node
  struct Node *rhs; // right hand side node
  int val;          // the value of this node (ND_NUM)
} Node;

typedef struct {
  void **data;
  int capacity;  // buffer allocated area
  int len;       // elements count
} Vector;

// generate assembly functions
// details: syntax.txt
extern Node *expr();
extern Node *equality();
extern Node *relational();
extern Node *add();
extern Node *mul();
extern Node *unary();
extern Node *term();
extern void gen(Node *node); // generate assembly

extern Vector *new_vector(); // create new vector
extern void vec_push(Vector *vec, void *elem); // push back in vector

// error functions
extern void error(char *fmt, ...); // error output
extern void error_at(char *loc, char *msg); // error output with error point in input expression
// tokenize input expression
extern void tokenize(char *p);
extern Node *new_node(int ty, Node *lhs, Node *rhs);
extern Node *new_node_num(int ty);
extern int consume(int ty);

extern Vector *tokens;
extern int pos;


