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

static int check_consecutive_pipes(t_lexer **cur)
{
    t_lexer *pt;

    pt = peek_token(cur);
    if (pt && pt->next && pt->next->tokentype == TOK_PIPE)
    {
        ft_putstr_fd("bash: syntax error near unexpected token `|'\n", STDERR_FILENO);
        return -1;
    }
    return 0;
}

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

static ast *parse_pipeline_1(t_lexer **cur, ast **left, int *n_pipes,
        t_minishell *minishell)
{
    ast *right;
    ast *node;

    while (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        if (check_consecutive_pipes(cur) == -1)
            return (free_ast(*left), NULL);
        consume_token(cur); // 消耗管道符号
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
 * parse_pipeline
 * ----------------
 * 目的：
 *   解析一条完整的管道命令，将多个通过 '|' 连接的简单命令
 *   构建成 PIPE 类型的 AST 树。
 *
 * 参数：
 *   - cur : 指向当前 token 游标的指针
 *
 * 返回值：
 *   - 成功：返回包含整个管道结构的 AST 根节点
 *   - 失败：解析失败时返回 NULL
 *
 * 行为说明：
 *   1. 首先调用 parse_simple_cmd() 解析管道最左侧的命令
 *   2. 调用 parse_pipeline_1() 解析右侧可能存在的管道，更新 AST
 *   3. 将管道数量 n_pipes 保存到 AST 根节点的 n_pipes 字段
 *   4. 返回 AST 根节点
 */
ast *parse_pipeline(t_lexer **cur, t_minishell *minishell)
{
    ast *left;
    int n_pipes;

    // 🚨 如果一开始就是 PIPE，直接报错
    if (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        ft_putstr_fd(
            "bash: syntax error near unexpected token `|'\n",
            STDERR_FILENO);
        return NULL;
    }
    left = parse_simple_cmd_redir_list(cur, minishell);
    if (!left)
        return NULL; // 其他语法错误，不是 PIPE
    n_pipes = 0;
    ast *result = parse_pipeline_1(cur, &left, &n_pipes, minishell);
    if (!result)
        return NULL;
    minishell->n_pipes = n_pipes;
    return result;
}
