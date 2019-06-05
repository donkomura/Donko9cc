#include "9cc.h"

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

Node *new_node_indent(int name) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_INDENT;
  node->name = name;
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
void program() {
  Token *token = tokens->data[pos];
  
  if (token->ty == TK_EOF) 
    return; 
  Node *node = stmt();
  vec_push(code, node);
  code_pos++;
  code->data[code_pos] = NULL;
  return program();
}

Node *stmt() {
  Node *node;
  Token *token = tokens->data[pos];
  if (consume(TK_RETURN)) {
    node = malloc(sizeof(node));
    node->ty = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }

  if (!consume(';'))
    error_at(token->input, "This token may be ';'");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume('='))
    node = new_node('=', node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume(TK_EQ))
      node = new_node(ND_EQ, node, relational());
    else if (consume(TK_NE))
      node = new_node(ND_NE, node, relational());
    else 
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume(TK_GE))
      node = new_node(ND_GE, node, add());
    else if (consume(TK_LE))
      node = new_node(ND_LE, node, add());
    else if (consume(TK_GT))
      node = new_node(ND_GT, node, add());
    else if (consume(TK_LT))
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

  if (token->ty == TK_NUM) {
    token = tokens->data[pos++];
    return new_node_num(token->val);
  }

  if (token->ty == TK_INDENT) {
    token = tokens->data[pos++];
    return new_node_indent(token->val);
  }
  token = tokens->data[pos++];
  printf("(char)%c (int)%d\n", token->ty, token->ty);
  error_at(token->input, "invalid token");
}

void gen_lval(Node *node) {
  if (node->ty != ND_INDENT) {
    error("left hand side value is not valiable.");
  }
  int offset = ('z' - node->name + 1) * 8; 
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", offset);
  printf("  push rax\n");
}

// generate assembly
void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_INDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if (node->ty == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  if (node->ty == '=') {
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
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

