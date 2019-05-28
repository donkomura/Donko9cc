#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

enum {
  TK_NUM = 256, // 整数
  TK_EOF,       // 終端
};

typedef struct {
  int ty;       // トークンの型
  int val;      // tyがTK_NUMの場合その数値
  char *input;  // トークン文字列(error)
} Token;

char *user_input;

Token tokens[100];

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *msg) {
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// トークンの分割
// tokensに保存
void tokenize() {
  char *p = user_input;
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    error_at(p, "Could not tokenize.");
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("invalid input.");
    return 1;
  }

  user_input = argv[1];
  tokenize();
  
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 式の最初は数値
  if (tokens[0].ty != TK_NUM) {
    error_at(tokens[0].input, "Not a Number.");
  }
  printf("  mov rax, %d\n", tokens[0].val);

  // `+ <数値>` または `- <数値>`
  // i = 0 は数値
  int i = 1;
  while (tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error_at(tokens[i].input, "Not a Number");
      printf("  add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    if (tokens[i].ty == '-') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error_at(tokens[i].input, "Not a Number");
      printf("  sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    error_at(tokens[i].input, "unexpected input");
  }

  printf("  ret\n");
  return 0;
}
