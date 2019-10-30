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
    Token *next;    // next input token
    int val;        // the number of TK_NUM
    char *str;      // token string
    int len;        // token length
};

// processing token
Token *token;

// program input
char *user_input;

// return the error and position
void error_at(char *loc, char *fmt, ...);

// if the next symbol is expected, read it in and keep going to next.
bool consume(char *op);

// If the next token meet the operater, read the token and
// keep going. Else waring as error.
void expect(char op);

// if next symbol is number, read it in and keep going to next.
// And then return the number, or warning if it's error.
int expect_number(void);

// input EOF
bool at_eof(void);

// build a new token and link to cur.
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

// analyze the string p and return the token linked-list
Token *tokenize(char *p);


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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

Node *new_node_num(int val);

Node *expr();
Node *mul();
Node *term();
Node *unary();

void gen(Node *node);


