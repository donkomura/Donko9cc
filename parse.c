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
      token->name = strndup(p, cnt);
      token->input = p;
      vec_push(tokens, token);
      p += cnt;
      continue;
    }

    if (strchr("+-*/>=<()!;", *p)) {
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

