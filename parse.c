#include "9cc.h"

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
bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
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
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1,sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
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
            cur = new_token(TK_RESERVED, cur, p++,1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        // error("Fail to tokenize!\n");

        // not correct symbol
        cur = new_token(TK_EOF, cur, p++, 0);
        error_at(cur->str,"Fail to tokenize!\n");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}