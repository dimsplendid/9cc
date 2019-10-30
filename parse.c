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
        
        if (!(memcmp(p, "==", 2) && memcmp(p, "!=", 2)
                && memcmp(p, ">=", 2) && memcmp(p, "<=", 2))) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }
        

        if (*p == '+' || *p == '-' || *p == '*'
                || *p == '/' || *p == '(' || *p == ')'
                || *p == '>' || *p == '<') {
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