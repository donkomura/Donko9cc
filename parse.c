#include "9cc.h"

static int blockCount;
static int argCount;

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int num) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = num;
  return node;
}

Node *new_node_indent(char *name) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
  map_set(map, name, offset_count++); 
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
  Token *token = tokens->data[pos];
  Node *node;

  if (consume('{')) {
    node = malloc(sizeof(Node));
    node->ty = ND_BLOCK;
    Vector *vec = new_vector();
    while (!consume('}')) {
      vec_push(vec, stmt());
      vec->data[++blockCount] = NULL;
    }
    node->block = (__typeof__(node->block))vec;
    return node;
  }

  if (consume(TK_IF)) {
    node = malloc(sizeof(Node));
    node->ty = ND_IF;
    if (consume('(')) {
      node->cond = expr();
      if (!consume(')')) {
        error_at(token->input, "invalid syntax: no ')'");
      }
    }

    node->then = stmt();

    if (consume(TK_ELSE)) {
      node->els = stmt();
    }
    return node;
  }

  if (consume(TK_WHILE)) {
    node = malloc(sizeof(Node));
    node->ty = ND_WHILE;
    if (consume('(')) {
      node->cond = expr();
      if (!consume(')')) {
        error_at(token->input, "invalid syntax: no ')'");
      }
    }

    node->body = stmt();
    return node;
  }

  if (consume(TK_FOR)) {
    node = malloc(sizeof(Node));
    node->ty = ND_FOR;
    if (consume('(')) {
      node->init = expr();
      if (!consume(';')) {
        error_at(token->input, "invalid syntax: no ')'");
      }
      node->cond = expr();
      if (!consume(';')) {
        error_at(token->input, "invalid syntax: no ')'");
      }
      node->inc = expr();
      if (!consume(')')) {
        error_at(token->input, "invalid syntax: no ')'");
      }
    }

    node->body = stmt();
    return node;
  }

  if (consume(TK_RETURN)) {
    node = malloc(sizeof(Node));
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
    return term();
  }
}

Node *term() {
  Token *token = tokens->data[pos];
  if (consume(TK_IDENT)) {
    Node *node = malloc(sizeof(Node));
    node->name = token->name;
    if (consume('(')) {
      node->ty = ND_FUNC;
      Vector *vec = new_vector();
      while (!consume(')')) {
        token = tokens->data[pos];
        if (consume(',')) continue;
        if (consume(TK_NUM)) {
          vec_pushi(vec, token->val);
        }
      }
      node->args = (__typeof__(node->args))vec;
      return node;
    }
    error_at(token->input, "expected ')'");
    return new_node_indent(token->name);
  }

  if (consume(TK_NUM)) {
    return new_node_num(token->val);
  }

  if (consume('(')) {
    Node *node = expr();
    if (!consume(')')) {
      error_at(token->input, "invalid syntax: no ')'");
    }
    return node;
  }

  token = tokens->data[pos++];
  error_at(token->input, "invalid token");
}
