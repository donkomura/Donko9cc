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

// 使用関数
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
void gen(Node *node);


char *user_input;  // 引数でとったトークン列

Token tokens[200];

int pos = 0;       // tokensのイテレータ

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *msg) {
  int posit = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", posit, "");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// トークンの分割
// tokensに保存
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strchr("+-*/>=<()!", *p)) {
      if ((*p == '=' || *p == '!') && *(p + 1) == '=') {
        if (*p == '=') {
          tokens[i].ty = ND_EQ;
        } else {
          tokens[i].ty = ND_NE;
        }
        tokens[i].input = p;
        i++;
        p += 2;
        continue;
      }

      if ((*p == '<' || *p == '>')) {
        if (*p == '<') {
          if (*(p + 1) == '=') {
            tokens[i].ty = ND_LE;
            i++;
            p += 2;
          } else {
            tokens[i].ty = ND_LT;
            i++;
            p++;
          }
        } else {
          if (*(p + 1) == '=') {
            tokens[i].ty = ND_GE;
            i++;
            p += 2;
          } else {
            tokens[i].ty = ND_GT;
            i++;
            p++;
          }
        }
        tokens[i].input = p;
        continue;
      }

      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = ND_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    error_at(p, "Could not tokenize.");
  }
  tokens[i].input = p;
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
  if (tokens[pos].ty != ty) 
    return 0;
  pos++;
  return 1;
}

// 抽象構文木のパーサー
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
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')')) {
      error_at(tokens[pos].input, "invalid syntax: no ')'");
    }
    return node;
  }

  if (tokens[pos].ty == ND_NUM)
    return new_node_num(tokens[pos++].val);
  error_at(tokens[pos].input, "invalid tokens");
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_EQ || node->ty == ND_NE \
      || node->ty == ND_GE || node->ty == ND_GT \
      || node->ty == ND_LE || node->ty == ND_LT) {
    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->ty) {
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
  }
  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "invalid input.\n");
    return 1;
  }
  
  user_input = argv[1];

  // tokenize
  tokenize(argv[1]);
  
  // 抽象構文木の構築
  Node *root = expr();
  
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 抽象構文木からコード生成
  gen(root);

  // スタックの先頭から計算結果を出力
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
