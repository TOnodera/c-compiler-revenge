#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 抽象構文木のノードの種類
typedef enum
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT,  // <
    ND_LE,  // <=
    ND_NUM, // NUM
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

// トークンの種類
typedef enum
{
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token
{
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

// 入力されるプログラムを保持する
char *user_input;

// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数をとる
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// ユーザー入力(user_input)のどこでエラーがあったかを指摘する
void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^ ");
    fprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

// 次のトークンが期待している記号のときは、トークンを１つ進めて
// 真を返す。それ以外の場合は偽を返す。
bool consume(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    {
        return false;
    }

    token = token->next;
    return true;
}

// 次のトークンが期待している記号の時は、トークンを読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    {
        error_at(token->str, "'%c'ではありません。", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合は、トークンを１つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number()
{
    if (token->kind != TK_NUM)
    {
        error_at(token->str, "数ではありません。");
    }
    int val = token->val;
    token = token->next;
    return val;
}

Node *
new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr()
{
    return equality();
}

Node *equality()
{
    Node *node = relational();
    for (;;)
    {
        if (consume("=="))
        {
            node = new_node(ND_ADD, node, relational());
        }
        else if (consume("!="))
        {
            node = new_node(ND_SUB, node, relational());
        }
        else
        {
            return node;
        }
    }
}

Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (consume("<"))
        {
            node = new_node(ND_LT, node, add());
        }
        else if (consume("<="))
        {
            node = new_node(ND_LE, node, add());
        }
        else if (consume(">"))
        {
            node = new_node(ND_LT, add(), node);
        }
        else if (consume(">="))
        {
            node = new_node(ND_LE, add(), node);
        }
        else
        {
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();
    if (consume("+"))
    {
        node = new_node(ND_ADD, node, mul());
    }
    else if (consume("-"))
    {
        node = new_node(ND_SUB, node, mul());
    }
    else
    {
        return node;
    }
}

Node *mul()
{
    Node *node = unary();
    if (consume("*"))
    {
        node = new_node(ND_DIV, node, unary());
    }
    else if (consume("/"))
    {
        node = new_node(ND_DIV, node, unary());
    }
    else
    {
        return node;
    }
}

Node *unary()
{
    if (consume("+"))
    {
        return unary();
    }
    if (consume("-"))
    {
        return new_node(ND_SUB, new_node_num(0), unary());
    }
    return primary();
}

Node *primary()
{
    if (consume("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    return new_node_num(expect_number());
}

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    }

    printf("  push rax\n");
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;

    return tok;
}

bool startswith(char *p, char *q)
{
    return memcmp(p, q, strlen(q)) == 0;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize()
{
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while (*p)
    {
        // 空白文字をスキップ
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">="))
        {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>", *p))
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません。\n");
        return 1;
    }

    // トークナイズする
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    // アセンブリ前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}