#include <stdio.h>
#include "parse.h"

/* 函数：print_argv（static 工具）
 * 作用：逐项打印 argv
 * 参数：argv —— 以 NULL 结尾的字符串数组
 * 返回：无
 * 在哪调用：print_cmd
 */
static void print_argv(char **argv)
{
    int i = 0;
    if (!argv) { printf("argv=NULL\n"); return; }
    while (argv[i])
    {
        printf("  argv[%d] = \"%s\"\n", i, argv[i]);   // 打印每一项
        i++;
    }
}

/* 函数：print_cmd（static 工具）
 * 作用：详细打印 CMD 结点的内容：argv 和各种重定向
 * 参数：n —— CMD 结点
 * 返回：无
 * 在哪调用：print_ast 的 NODE_CMD 分支
 */
static void print_cmd(ast *n)
{
    print_argv(n->argv);                                // 打印 argv
    if (n->redir_in)     printf("  <  %s\n", n->redir_in);
    if (n->redir_out)    printf("  >  %s\n", n->redir_out);
    if (n->redir_append) printf("  >> %s\n", n->redir_append);
    if (n->heredoc_delim)
        printf("  << %s%s\n", n->heredoc_delim, n->heredoc_quoted ? " (quoted)" : "");
}

/* 函数：print_ast
 * 作用：详细打印整棵 AST（用于调试/单测）
 * 参数：node —— AST 根
 * 返回：无
 * 在哪调用：解析后调试输出；或单元测试对拍。
 */
void print_ast(ast *node)
{
    if (!node) { printf("<null>\n"); return; }          // 空树

    switch (node->type)
    {
        case NODE_CMD:
            printf("[CMD]\n");
            print_cmd(node);                            // 打印命令细节
            break;

        case NODE_PIPE:
            printf("[PIPE]\nL ->\n");
            print_ast(node->left);                      // 递归打印左
            printf("R ->\n");
            print_ast(node->right);                     // 递归打印右
            break;

        case NODE_AND:
            printf("[AND]\nL ->\n");
            print_ast(node->left);
            printf("R ->\n");
            print_ast(node->right);
            break;

        case NODE_OR:
            printf("[OR]\nL ->\n");
            print_ast(node->left);
            printf("R ->\n");
            print_ast(node->right);
            break;

        case NODE_SEQUENCE:
            printf("[SEQUENCE]\nL ->\n");
            print_ast(node->left);
            printf("R ->\n");
            print_ast(node->right);
            break;

        case NODE_SUBSHELL:
            printf("[SUBSHELL] (\n");
            print_ast(node->sub);                       // 括号内内容
            printf(")\n");
            break;

        case NODE_BACKGROUND:
            printf("[BACKGROUND flag=%d]\n", node->is_background);
            break;

        default:
            printf("[UNKNOWN]\n");
            break;
    }
}
