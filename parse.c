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

/*
1. == !=
2. < <= > >=
3. + -
4. * /
5. unary +, unary -
6. ()
expr        = equality
equality    = relational ("==" relational | "!=" relational)*
relational  = add ("<" add | "<=" add | ">" add | ">=" add)*
add = mul("+" mul | "-" mul)*
mul = unary("*" unary | "/" unary)*
unary = ("+" | "-")? term
term = num | "(" expr ")"
> compare -> to be implemented
*/

Node *expr() {
    Node *node = equality();
    return node;
}

Node *equality() {
    Node *node = relational();
    for(;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NEQ, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();
    for(;;) {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume("<="))
            node = new_node(ND_NGT, node, add());
        else if (consume(">"))
            node = new_node(ND_GT, node, add());
        else if (consume(">="))
            node = new_node(ND_NLT, node, add());
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;        
    }
}

Node *mul() {
    Node *node = unary();

    for(;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
        
    }
}

Node *term() {
    // If the next token is "(", it should be "(" expr ")"
    if (consume("(")) {
        Node *node = expr();
        expect(')');
        return node;
    }

    // nor it should be a number
    return new_node_num(expect_number());
}

Node *unary() {
    if (consume("+"))
        return term();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), term());
    return term();
}