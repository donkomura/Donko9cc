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

    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      token->ty = TK_WHILE;
      token->input = p;
      vec_push(tokens, token);
      p += 5;
      continue;
    }

    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      token->ty = TK_FOR;
      token->input = p;
      vec_push(tokens, token);
      p += 3;
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

    if (strchr("+-*/>=<()!;{},", *p)) {
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
        p++;
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
      token->val = atoi(p);
      vec_push(tokens, token);
      p++;
      continue;
    }

    error_at(p, "Could not tokenize.");
  }

  Token *token = malloc(sizeof(Token));
  token->ty = TK_EOF;
  token->input = p;
  vec_push(tokens, token);
}
