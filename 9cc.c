/*
 * =====================================================================================
 *
 *       Filename:  9cc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/04/2019 02:35:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 *  https://www.sigbus.info/compilerbook/
 * =====================================================================================
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする


// トークンの型を表す値
enum {
    TK_NUM = 256, // 整数トークン
    TK_EOF,       // 入力の終わりを表すトークン
};

#ifdef new_ver
enum {
    ND_NUM = 256, // 整数のノードの型
};
#endif

// トークンの型
typedef struct {
    int ty;      // トークンの型
    int val;     // tyがTK_NUMの場合、その数値
    char *input; // トークン文字列（エラーメッセージ用）
} Token;
Token tokens[100];

#ifdef new_ver

//syntax tree
typedef struct Node {
  int ty;           // 演算子かND_NUM
  struct Node *lhs; // left-hand side
  struct Node *rhs; // right-hand side
  int val;          // tyがND_NUMの場合のみ使う
} Node;
Node tokens_node[256];

//build node and set lhs and rhs and ty(type) 二項演算子
Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;  //set type
    node->lhs = lhs;//set lhs
    node->rhs = rhs;//set rhs
    return node;
}

//build number 数字
Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;//substitute type
    node->val = val;
    return node;
}

//
int consume(int ty) {
    if (tokens[pos].ty != ty)
        return 0;
    pos++;
    return 1;
}

Node *term() {
    if (consume('(')) {
        Node *node = add();
        if (!consume(')'))
            error("開きカッコに対応する閉じカッコがありません: %s",
            tokens[pos].input);
        return node;
    }

    if (tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);

    error("数値でも開きカッコでもないトークンです: %s"
    tokens[pos].input);
}

Node *mul() {
    Node *node = term();

    for (;;) {
        if (consume('*'))
            node = new_node('*', node, term());
        else if (consume('/'))
            node = new_node('/', node, term());
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}
#endif

//tokenize ver2 is to make syntax tree for (, * and /
void tokenize_ver2(char *p) {
    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '?') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        if (*p == '+' ) {
            tokens_node = new_node();
        }




        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}












// pが指している文字列をトークンに分割してtokensに保存する
void tokenize(char *p) {
    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '?') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}





// エラーを報告するための関数
void error(int i) {
    fprintf(stderr, "予期しないトークンです: %s\n",
            tokens[i].input);
    exit(1);
}




int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }




#ifdef new_ver
    tokenize_ver2(argv[1]);
#else
    // トークナイズする
    tokenize(argv[1]);
#endif



    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 式の最初は数でなければならないので、それをチェックして
    // 最初のmov命令を出力
    if (tokens[0].ty != TK_NUM)
        error(0);
    printf("  mov rax, %d\n", tokens[0].val);




    // `+ <数>`あるいは`- <数>`というトークンの並びを消費しつつ
    // アセンブリを出力
    int i = 1;
    while (tokens[i].ty != TK_EOF) {
        if (tokens[i].ty == '+') {
            i++;
            if (tokens[i].ty != TK_NUM)
                error(i);
            printf("  add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        if (tokens[i].ty == '-') {
            i++;
            if (tokens[i].ty != TK_NUM)
                error(i);
            printf("  sub rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        if (tokens[i].ty == '?') {
            i++;
            if (tokens[i].ty != TK_NUM)
                error(i);
            printf("  add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        error(i);
    }

    printf("  ret\n");
    return 0;
}
