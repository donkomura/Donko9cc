#include "9cc.h"

char *user_input;
int pos = 0, code_pos = 0;

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
  code = new_vector();

  user_input = argv[1];
  
  // tokenize
  tokenize(argv[1]);
  // store in `code`
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prorogue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // generate assembly for each `code` element
  for (int i = 0; i < code->len; i++) {
    gen(code->data[i]);
    printf("  pop rax\n");
  }

  // epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
