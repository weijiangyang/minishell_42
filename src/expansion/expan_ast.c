/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+         +:+         */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+         +#+        */
/*                                                                            */
/* ************************************************************************** */


#include "expander.h"
#include "minishell.h"

/* 是否为 export 命令：argv[0] 精确等于 "export" */
static int  is_export_cmd(ast *node)
{
    if (!node || node->type != NODE_CMD || !node->argv || !node->argv[0])
        return (0);
    return (ft_strncmp(node->argv[0], "export", 7) == 0);
}

/* 处理命令节点的 argv：export 保留引号，非 export 去壳 */
static int  expand_argv_of_cmd(t_minishell *msh, ast *node, int export_mode)
{
    int     i;
    char    *tmp;

    i = 0;
    while (node->argv && node->argv[i])
    {
        if (export_mode)
        {
            tmp = expand_all(msh, node->argv[i]);     /* 仅 $ 展开，保留引号 */
            if (!tmp)
                return (0);
            free(node->argv[i]);
            node->argv[i] = tmp;
        }
        else
        {
            tmp = expander_str(msh, node->argv[i]);   /* 展开 + 去引号 */
            if (!tmp)
                return (0);
            node->argv[i] = tmp;
        }
        i++;
    }
    return (1);
}

/* 重定向/heredoc：保持原策略（不做字段分裂/通配），只用已有工具 */
static int  expand_redirs_of_cmd(t_minishell *msh, ast *node)
{
    char    *tmp;
    int     had_q, q_s, q_d;

    if (node->redir_in)
    {
        tmp = expander_str(msh, node->redir_in);
        if (!tmp)
            return (0);
        node->redir_in = tmp;
    }
    if (node->redir_out)
    {
        tmp = expander_str(msh, node->redir_out);
        if (!tmp)
            return (0);
        node->redir_out = tmp;
    }
    if (node->redir_append)
    {
        tmp = expander_str(msh, node->redir_append);
        if (!tmp)
            return (0);
        node->redir_append = tmp;
    }
    if (node->heredoc_delim)
    {
        if (node->heredoc_quoted) /* 只去壳，不做变量展开 */
        {
            tmp = remove_quotes_flag(node->heredoc_delim, &had_q, &q_s, &q_d);
            if (!tmp)
                tmp = ft_strdup(node->heredoc_delim);
            if (!tmp)
                return (0);
            free(node->heredoc_delim);
            node->heredoc_delim = tmp;
        }
        else                      /* 展开 + 去壳 */
        {
            tmp = expander_str(msh, node->heredoc_delim);
            if (!tmp)
                return (0);
            node->heredoc_delim = tmp;
        }
    }
    return (1);
}

/* 处理一个命令节点 */
static int  expand_cmd_node(t_minishell *msh, ast *node)
{
    int export_mode;

    export_mode = is_export_cmd(node);
    if (!expand_argv_of_cmd(msh, node, export_mode))
        return (0);
    if (!expand_redirs_of_cmd(msh, node))
        return (0);
    return (1);
}

/* 基于 AST 的 DFS 展开 */
static int  expand_dfs(t_minishell *msh, ast *node)
{
    if (!node)
        return (1);
    if (node->type == NODE_CMD)
        if (!expand_cmd_node(msh, node))
            return (0);
    if (node->left)
        if (!expand_dfs(msh, node->left))
            return (0);
    if (node->right)
        if (!expand_dfs(msh, node->right))
            return (0);
    if (node->sub)
        if (!expand_dfs(msh, node->sub))
            return (0);
    return (1);
}

/* 新入口：parse 之后 / exec 之前调用 */
int expander_ast(t_minishell *minishell, ast *root)
{
    return (expand_dfs(minishell, root));
}
