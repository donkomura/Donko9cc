#include "9cc.h"

char *gen_label(char* type) {
  static int n;
  static int begin;
  static int end;
  char buff[10];
  if (strcmp(type, "begin") == 0)
    sprintf(buff, ".LB%d", begin++);
  else if (strcmp(type, "end") == 0) 
    sprintf(buff, ".LE%d", end++);
  else if (strcmp(type, "if") == 0)
    sprintf(buff, ".L%d", n++);
  return strdup(buff);
}

void gen_lval(Node *node) {
  if (node->ty != ND_IDENT) {
    error("left hand side value is not valiable.");
  }
  int offset = (int)map_get(map, node->name) * 8; 
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", offset);
  printf("  push rax\n");
  return;
}

// generate assembly
void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_BLOCK) {
    Vector *vec = node->block;
    for (int i = 0; i < vec->len; i++) {
      gen(vec->data[i]);
      printf("  pop rax\n");
    }
    return;
  }

  if (node->ty == ND_IDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if (node->ty == ND_IF) {
    char *ifLabel = gen_label("if");
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  %s\n", ifLabel);

    gen(node->then);

    printf("%s:\n", ifLabel);
    return;
  }

  if (node->ty == ND_WHILE) {
    char *beginLabel = gen_label("begin");
    char *endLabel = gen_label("end");
    printf("%s:", beginLabel);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  %s\n", endLabel);
    
    gen(node->body);

    printf("  jmp %s\n", beginLabel);
    printf("%s:\n", endLabel);
    return;
  }

  if (node->ty == ND_FOR) {
    char *beginLabel = gen_label("begin");
    char *endLabel = gen_label("end");
    gen(node->init);

    printf("%s:\n", beginLabel);

    gen(node->cond);

    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je %s\n", endLabel);

    gen(node->body);
    gen(node->inc);

    printf("  jmp %s\n", beginLabel);
    printf("%s:\n", endLabel);
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
      break;
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
      break;
  }
  printf("  push rax\n");
}

