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

    if ('a' <= *p && *p <= 'z') {
      token->ty = TK_INDENT;
      token->input = p;
      vec_push(tokens, token);
      p++;
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

