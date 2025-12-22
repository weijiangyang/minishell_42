#include <stdio.h>
#include "parse.h"

/* 函数：type_name（static 内部工具）
 * 作用：把枚举 node_type 转为可读字符串，便于调试查看
 * 参数：t —— 结点类型
 * 返回：常量字符串
 * 在哪调用：print_ast_by_type
 */
static const char *type_name(node_type t)
{
    switch (t)
    {
        case NODE_CMD:        return "CMD";
        case NODE_PIPE:       return "PIPE";
        case NODE_AND:        return "AND";
        case NODE_OR:         return "OR";
        case NODE_SUBSHELL:   return "SUBSHELL";
        case NODE_BACKGROUND: return "BACKGROUND";
        case NODE_SEQUENCE:   return "SEQUENCE";
        default:              return "?";
    }
}

/* 函数：print_ast_by_type
 * 作用：按“结构形态”打印 AST（只打印类型，不细化到具体 argv/重定向内容）
 * 参数：node —— AST 根
 * 返回：无
 * 在哪调用：调试时快速确认树形结构与优先级折叠是否正确。
 */
void print_ast_by_type(ast *node)
{
    if (!node) { printf("<null>\n"); return; }          // 空树打印 <null>

    printf("[%s]", type_name(node->type));              // 打印类型名

    if (node->type == NODE_CMD)                         // CMD：打印个简要的 argc
    {
        int argc = 0;
        if (node->argv) { while (node->argv[argc]) argc++; }
        printf(" argc=%d\n", argc);
        return;
    }

    if (node->type == NODE_SUBSHELL)                    // SUBSHELL：缩进打印子树
    {
        printf(" (\n");
        print_ast_by_type(node->sub);
        printf(")\n");
        return;
    }

    // 其它二叉结点：递归打印左右子树
    printf("\nL: ");
    print_ast_by_type(node->left);
    printf("R: ");
    print_ast_by_type(node->right);
}
