#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

enum {
  ND_NUM = 256, // 整数
  ND_EQ, // ==
  ND_NE, // != 
  ND_LE, // <=
  ND_GE, // >=
  ND_LT, // <
  ND_GT  // >
};

typedef struct {
  void **data;
  int capacity;  // buffer allocated area
  int len;       // elements count
} Vector;

typedef struct {
  int ty;       // トークンの型
  int val;      // tyがND_NUMの場合その数値
  char *input;  // トークン文字列(error)
} Token;

typedef struct Node {
  int ty;           // 演算子日ND_NUM
  struct Node *lhs;
  struct Node *rhs;
  int val;          // tyがND_NUMのときの数値
} Node;

// generate assembly functions
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
void gen(Node *node);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);

int pos;
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

Vector *tokens;

int pos = 0;       // tokensのイテレータ

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

// split token
// save tokens at [tokens]
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    Token *token = malloc(sizeof(Token));

    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strchr("+-*/>=<()!", *p)) {
      if ((*p == '=' || *p == '!') && *(p + 1) == '=') {
        if (*p == '=') {
          token->ty = ND_EQ;
        } else {
          token->ty = ND_NE;
        }
        token->input = p;
        vec_push(tokens, token);
        p += 2;
        continue;
      }

      if ((*p == '<' || *p == '>')) {
        if (*p == '<') {
          if (*(p + 1) == '=') {
            token->ty = ND_LE;
            p += 2;
          } else {
            token->ty = ND_LT;
            p++;
          }
        } else {
          if (*(p + 1) == '=') {
            token->ty = ND_GE;
            p += 2;
          } else {
            token->ty = ND_GT;
            p++;
          }
        }
        token->input = p;
        vec_push(tokens, token);
        continue;
      }

      token->ty = *p;
      token->input = p;
      vec_push(tokens, token);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      token->ty = ND_NUM;
      token->input = p;
      token->val = strtol(p, &p, 10);
      vec_push(tokens, token);
      continue;
    }

    error_at(p, "Could not tokenize.");
  }

  Token *token = malloc(sizeof(Token));
  token->input = p;
  vec_push(tokens, token);
}

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int ty) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = ty;
  return node;
}

int consume(int ty) {
  Token *token = tokens->data[pos]; 
  if (token->ty != ty) 
    return 0;
  pos++;
  return 1;
}

// parser abstract syntax tree
Node *expr() {
  return equality();
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume(ND_EQ))
      node = new_node(ND_EQ, node, relational());
    else if (consume(ND_NE))
      node = new_node(ND_NE, node, relational());
    else 
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume(ND_GE))
      node = new_node(ND_GE, node, add());
    else if (consume(ND_LE))
      node = new_node(ND_LE, node, add());
    else if (consume(ND_GT))
      node = new_node(ND_GT, node, add());
    else if (consume(ND_LT))
      node = new_node(ND_LT, node, add());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume('*')) 
      node = new_node('*', node, unary());
    else if (consume('/'))
      node = new_node('/', node, unary());
    else
      return node;
  }
}

Node *unary() {
  for (;;) {
    if (consume('+')) {
      return term();
    }
    if (consume('-')) {
      return new_node('-', new_node_num(0), term());
    }
    return term();       // 単項演算子なし
  }
}

Node *term() {
  Token *token = tokens->data[pos];
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')')) {
      error_at(token->input, "invalid syntax: no ')'");
    }
    return node;
  }

  if (token->ty == ND_NUM) {
    token = tokens->data[pos++];
    return new_node_num(token->val);
  }
  token = tokens->data[pos++];
  error_at(token->input, "invalid tokens");
}

// generate assembly
void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->ty) {
    case '+':
      printf("  add rax, rdi\n");
      break;
    case '-':
      printf("  sub rax, rdi\n");
      break;
    case '*':
      printf("  imul rdi\n");
      break;
    case '/':
      printf("  cqo\n");
      printf("  idiv rdi\n");
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl  al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_GE:
      printf("  cmp rdi, rax\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_GT:
      printf("  cmp rdi, rax\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
  }
  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc!= 2) {
    fprintf(stderr, "invalid input.\n");
    return 1;
  }

  if (strcmp(argv[1], "-test") == 0) {
    runtest();
    return 0;
  }

  tokens = new_vector();
  
  user_input = argv[1];

  // tokenize
  tokenize(argv[1]);
  
  // build abstract syntax tree
  Node *root = expr();
  
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // generate assembly
  gen(root);

  // output results
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
