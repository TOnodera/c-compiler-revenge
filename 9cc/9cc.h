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

// 宣言
// 入力されるプログラムを保持する
extern char *user_input;
// 現在着目しているトークン
extern Token *token;

void error(char *fmt, ...);
// ユーザー入力(user_input)のどこでエラーがあったかを指摘する
void error_at(char *loc, char *fmt, ...);
bool at_eof();
// 次のトークンが期待している記号のときは、トークンを１つ進めて
// 真を返す。それ以外の場合は偽を返す。
bool consume(char *op);
// 次のトークンが期待している記号の時は、トークンを読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op);
// 次のトークンが数値の場合は、トークンを１つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);
// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
// 入力文字列pをトークナイズしてそれを返す
Token *tokenize();