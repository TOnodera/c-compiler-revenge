#include "9cc.h"

// 定義(宣言はヘッダーファイルにある)
char *user_input;
Token *token;
Node *code[100];

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
    program();
    // アセンブリ前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 変数26個分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for (int i = 0; code[i]; i++)
    {
        gen(code[i]);
        printf("  pop rax\n");
    }

    printf("  mov rsp,rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}