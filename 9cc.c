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

        if (*p == '+' || *p == '-' || *p == '*'
                || *p == '/' || *p == '(' || *p == ')') {
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

// EBNF
// kinds of abstract syntax tree
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

// structure of abstract
struct Node {
    NodeKind kind;  // kind of node
    Node *lhs;      // left hand side
    Node *rhs;      // right hand side
    int val;        // only using for ND_NUM kin
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *term();

Node *expr() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (consume('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;        
    }
}

Node *mul() {
    Node *node = term();

    for(;;) {
        if (consume('*'))
            node = new_node(ND_MUL, node, term());
        else if (consume('/'))
            node = new_node(ND_DIV, node, term());
        else
            return node;
        
    }
}

Node *term() {
    // If the next token is "(", it should be "(" expr ")"
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    // nor it should be a number
    return new_node_num(expect_number());
}

void gen(Node *node);
void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind) {
        case ND_ADD:
            printf("   add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
    }

    printf("    push rax\n");
}

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