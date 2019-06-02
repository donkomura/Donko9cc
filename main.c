#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

Vector *tokens;
char *user_input;
int pos = 0;

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
