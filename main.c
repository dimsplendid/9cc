#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "The number of argument parameter is wrong!\n");

        return 1;
    }

    // token analysis
    user_input = argv[1];
    token = tokenize(argv[1]);
    Node *node = expr();

    // output the formor comand of assembly lang
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // output the command by climbing syntax tree
    gen(node);

    // let the result stay in the top of stack
    // and pop to rax as return value
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}