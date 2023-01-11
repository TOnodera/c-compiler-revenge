#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 抽象構文木のノードの種類
typedef enum
{
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // <
    ND_LE,      // <=
    ND_ASSIGN,  // =
    ND_LVAR,    // ローカル変数
    ND_NUM,     // NUM
    ND_RETURN,  // returnを表すノード
    ND_IF,      // if
    ND_WHILE,   // while
    ND_FOR,     // for
    ND_BLOCK,   // Block
    ND_FUNCALL, // 関数呼び出し
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *next;

    // "if" "while" "for" statement
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;

    // ブロック呼び出し
    Node *body;

    // 関数名
    char *funcname;

    int val;
    int offset;
};

// トークンの種類
typedef enum
{
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
    TK_RETURN,   // returnを表すトークン
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

typedef struct LVar LVar;

struct LVar
{
    LVar *next;
    char *name;
    int len;
    int offset; // RBPからのoffset
};

extern LVar *locals;

// 宣言
// 入力されるプログラムを保持する
extern char *user_input;
// 現在着目しているトークン
extern Token *token;

extern Node *code[100];
void error(char *fmt, ...);
// ユーザー入力(user_input)のどこでエラーがあったかを指摘する
void error_at(char *loc, char *fmt, ...);
bool at_eof();
// 次のトークンが期待している記号のときは、トークンを１つ進めて
// 真を返す。それ以外の場合は偽を返す。
bool consume(char *op);
Token *consume_ident();
// 次のトークンが期待している記号の時は、トークンを読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op);
// 次のトークンが数値の場合は、トークンを１つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);
void gen_lval(Node *node);
LVar *find_lvar(Token *tok);
// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
// 入力文字列pをトークナイズしてそれを返す
Token *tokenize();
// 入力文字列がアルファベット、数値、アンダーバーかどうか判定する
bool is_alnum(char c);
bool is_alpha(char c);
