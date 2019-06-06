#include "9cc.h"

// split token
// save tokens at [tokens]
void tokenize(char *p) {
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    Token *token = malloc(sizeof(Token));

    // operation words
    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      token->ty = TK_IF;
      token->input = p;
      vec_push(tokens, token);
      p += 2;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      token->ty = TK_ELSE;
      token->input = p;
      vec_push(tokens, token);
      p += 4;
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      token->ty = TK_RETURN;
      token->input = p;
      p += 6;
      continue;
    }

    if (('A' <= *p && *p <= 'Z') || (*p == '_') || ('a' <= *p && *p <= 'z')) {
      int cnt = 1;
      while (isalpha(p[cnt]) || isdigit(p[cnt]) || p[cnt] == '_') cnt++;
      token->ty = TK_IDENT;
      token->name = strndup(p, (size_t)cnt);
      token->input = p;
      vec_push(tokens, token);
      p += cnt;
      continue;
    }

    if (strchr("+-*/>=<()!;", *p)) {
      if ((*p == '=' || *p == '!') && *(p + 1) == '=') {
        if (*p == '=') {
          token->ty = TK_EQ;
        } else {
          token->ty = TK_NE;
        }
        token->input = p;
        vec_push(tokens, token);
        p += 2;
        continue;
      }

      if ((*p == '<' || *p == '>')) {
        if (*p == '<') {
          if (*(p + 1) == '=') {
            token->ty = TK_LE;
            p += 2;
          } else {
            token->ty = TK_LT;
            p++;
          }
        } else {
          if (*(p + 1) == '=') {
            token->ty = TK_GE;
            p += 2;
          } else {
            token->ty = TK_GT;
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
      token->ty = TK_NUM;
      token->input = p;
      token->val = strtol(p, &p, 10);
      vec_push(tokens, token);
      continue;
    }

    error_at(p, "Could not tokenize.");
  }

  Token *token = malloc(sizeof(Token));
  token->ty = TK_EOF;
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

Node *new_node_indent(char *name) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
  map_set(map, name, (void *)offset_count++); 
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

  if (consume(TK_RETURN)) {
    printf("token: %c", *(token->input));
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

  if (token->ty == TK_IDENT) {
    token = tokens->data[pos++];
    return new_node_indent(token->name);
  }
  token = tokens->data[pos++];
  error_at(token->input, "invalid token");
}


