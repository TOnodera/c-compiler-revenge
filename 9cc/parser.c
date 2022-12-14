#include "9cc.h"

// 次のトークンが期待している記号のときは、トークンを１つ進めて
// 真を返す。それ以外の場合は偽を返す。
bool consume(char *op)
{
    if ((token->kind != TK_RESERVED && token->kind != TK_RETURN) || strlen(op) != token->len || memcmp(token->str, op, token->len))
    {
        return false;
    }

    token = token->next;
    return true;
}

Token *consume_ident()
{
    if (token->kind == TK_IDENT)
    {
        Token *ident = token;
        token = token->next;
        return ident;
    }
    return NULL;
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

bool is_alpha(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           (c == '_');
}

bool is_alnum(char c)
{
    return is_alpha(c) ||
           ('0' <= c && c <= '9');
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

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
        {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        if (!strncmp(p, "while", 5))
        {
            cur = new_token(TK_RESERVED, cur, p, 5);
            p += 5;
            continue;
        }

        if (!strncmp(p, "else", 4))
        {
            cur = new_token(TK_RESERVED, cur, p, 4);
            p += 4;
            continue;
        }

        if (!strncmp(p, "for", 3))
        {
            cur = new_token(TK_RESERVED, cur, p, 3);
            p += 3;
            continue;
        }

        if (!strncmp(p, "if", 2))
        {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">="))
        {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>;={}", *p))
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (is_alpha(*p))
        {
            char *q = p++;
            while (is_alnum(*p))
            {
                p++;
            }
            cur = new_token(TK_IDENT, cur, q, p - q);
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