#include "9cc.h"

// 定義(宣言はヘッダーファイルにある)
char *user_input;
Token *token;

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