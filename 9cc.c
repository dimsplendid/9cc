#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// kinds of tokens
typedef enum {
    TK_RESERVED,    // symbols
    TK_NUM,         // integer token
    TK_EOF,         // token of end of file
} TokenKind;

typedef struct Token Token;
// Token type
struct Token {
    TokenKind kind; // kind of token
    Token *next;   // next input token
    int val;        // the number of TK_NUM
    char *str;      // token string
};

// processing token
Token *token;

// handle erorr function
// parameters are the same with printf
// void error(char *fmt, ...) {
//     va_list ap;
//     va_start(ap, fmt);
//     vfprintf(stderr, fmt, ap);
//     fprintf(stderr, "\n");
//     exit(1);
// }

// program input
char *user_input;

// return the wrong position
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // print 'pos' number of space
    fprintf(stderr,"^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// if the next symbol is expected, read it in and keep going to next.
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// If the next token meet the operater, read the token and
// keep going. Else waring as error.
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        // error("Not '%c'\n", op);
        error_at(token->str, "Not '%c'\n", op);
    }
    token = token->next;
}

// if next symbol is number, read it in and keep going to next.
// And then return the number, or warning if it's error.
int expect_number(void) {
    if(token->kind != TK_NUM) {
        // error("Not a number!\n");
        error_at(token->str, "Not a number!\n");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(void){
    return token->kind == TK_EOF;
}

// build a new token and link to cur.
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1,sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// analyze the string p and return the token linked-list
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token * cur = &head;

    while (*p) {
        // escape space symbol
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        // error("Fail to tokenize!\n");

        // not correct symbol
        cur = new_token(TK_EOF, cur, p++);
        error_at(cur->str,"Fail to tokenize!\n");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "The number of argument parameter is wrong!\n");

        return 1;
    }

    // input parameter
    user_input = argv[1];
    // tokenize
    token = tokenize(argv[1]);

    // output the formor comand of assembly lang
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // test if the formula start with number
    // output the initial number
    printf("    mov rax, %d\n", expect_number());

    // comsume '+ <number>' or '- <number>'
    // and then output with assembly lang
    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n",expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }

    printf("  ret\n");
    return 0;
}