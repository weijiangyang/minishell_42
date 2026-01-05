/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:17:29 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:17:37 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../../include/minishell.h"
#include "../../include/parse.h"
#include "../../include/error.h"
#include "../../libft/libft.h"

/**
 * @brief 检查词法流中是否存在非法的连续管道符。
 * * 该函数通过 peek_token 查看当前 Token 及其后续节点：
 * 1. 如果当前 Token 之后紧跟另一个 TOK_PIPE，则判定为语法错误。
 * 2. 这种检查能有效拦截类似于 "ls ||" 或 "| |" 的非法输入（注意：Minishell 通常不处理 || 逻辑运算符）。
 * 3. 报错信息模拟了 Bash 的标准输出格式。
 * * @param cur 指向当前词法 Token 流指针的地址。
 * @return int 发现非法连续管道返回 -1；语法正常返回 0。
 */
static int check_consecutive_pipes(t_lexer **cur)
{
    t_lexer *pt;

    pt = peek_token(cur);
    if (pt && pt->next && pt->next->tokentype == TOK_PIPE)
    {
        ft_putstr_fd("bash: syntax error near unexpected token `|'\n", STDERR_FILENO);
        return (-1);
    }
    return 0;
}

/**
 * @brief 创建并初始化一个管道类型的 AST 节点。
 * * 该函数用于构建树状结构的中间节点：
 * 1. 分配 ast 结构体的内存并清零。
 * 2. 设置节点类型为 NODE_PIPE。
 * 3. 建立父子关系：将左侧分支（通常是已解析的命令）和右侧分支连接到该节点。
 * 4. 计数器递增：更新 pipeline 中管道的总数，用于后续执行器分配管道数组。
 * * @param left    指向管道左侧子树的指针。
 * @param right   指向管道右侧子树的指针。
 * @param n_pipes 指向管道计数器的指针，用于统计整行命令中的管道数量。
 * @return ast* 成功则返回新创建的管道节点；分配失败返回 NULL。
 */
static ast *create_pipe_node(ast *left, ast *right, int *n_pipes)
{
    ast *node;

    node = ft_calloc(1, sizeof(ast));
    if (!node)
        return NULL;
    node->type = NODE_PIPE;
    node->left = left;
    node->right = right;
    (*n_pipes)++;
    return node;
}

/**
 * @brief 循环解析管道线中的后续部分。
 * * 该函数处理形如 "cmd1 | cmd2 | cmd3" 中的 "| cmd2 | cmd3" 部分：
 * 1. 检查是否存在连续非法的管道符 (check_consecutive_pipes)。
 * 2. 消耗管道符 (TOK_PIPE) 并递归解析其右侧的简单命令。
 * 3. 语法验证：如果管道符后没有命令（例如 "ls |"），报错并返回 NULL。
 * 4. 节点构建：调用 create_pipe_node 将之前的树 (*left) 与新命令 (right) 连接。
 * 5. 更新迭代：将新生成的管道节点设为下一次循环的左侧节点，实现树的向上生长。
 * * @param cur        指向当前词法 Token 流指针的地址。
 * @param left       指向当前已构建好的左侧子树指针的地址。
 * @param n_pipes    指向管道总计数器的指针。
 * @param minishell  指向全局上下文结构体。
 * @return ast* 返回构建完成的管道线 AST 根节点；解析失败则返回 NULL。
 */
static ast *parse_pipeline_1(t_lexer **cur, ast **left, int *n_pipes,
        t_minishell *minishell)
{
    ast *right;
    ast *node;

    while (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        if (check_consecutive_pipes(cur) == -1)
            return (free_ast(*left), NULL);
        consume_token(cur);
        right = parse_simple_cmd_redir_list(cur, minishell);
        if (!right)
        {
            ms_err_syntax_unexpected("newline");
            minishell->last_exit_status = 2;
            return (free_ast(*left), NULL);
        }
        node = create_pipe_node(*left, right, n_pipes);
        if (!node)
            return (free_ast(*left),free_ast(right), NULL);
        *left = node;
    }
    return *left;
}

/**
 * @brief 管道链解析的入口函数。
 * * 该函数负责协调整条管道线的解析流程：
 * 1. 语法检查：拦截以管道符 `|` 开头的非法输入（例如输入 `| ls`）。
 * 2. 基础解析：调用 parse_simple_cmd_redir_list 解析第一个命令单元（左侧节点）。
 * 3. 循环解析：调用 parse_pipeline_1 处理后续可能存在的管道连接（| cmd2 | cmd3...）。
 * 4. 计数同步：解析完成后，将统计到的管道总数 (n_pipes) 存入全局结构体供执行器使用。
 * * @param cur        指向当前词法 Token 流指针的地址。
 * @param minishell  指向全局上下文结构体。
 * @return ast* 返回构建完整的管道线 AST 根节点；若解析失败或语法错误则返回 NULL。
 */
ast *parse_pipeline(t_lexer **cur, t_minishell *minishell)
{
    ast *left;
    int n_pipes;

    if (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        ft_putstr_fd(
            "bash: syntax error near unexpected token `|'\n",
            STDERR_FILENO);
        return NULL;
    }
    left = parse_simple_cmd_redir_list(cur, minishell);
    if (!left)
        return NULL;
    n_pipes = 0;
    ast *result = parse_pipeline_1(cur, &left, &n_pipes, minishell);
    if (!result)
        return NULL;
    minishell->n_pipes = n_pipes;
    return result;
}
