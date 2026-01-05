/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_simple_cmd_redir_list.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:10 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:19:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/parse.h"
#include "../../libft/libft.h"

/**
 * parse_simple_cmd_redir_list
 * ------------------------------------------------------------
 * 目的：
 *   解析一个简单命令（可能带重定向或子 shell），构建对应的 AST 节点。
 *   - 如果是子 shell（以 '(' 开始） → 调用 parse_subshell
 *   - 否则 → 调用 parse_normal_cmd_redir_list 解析普通命令及重定向
 *
 * 参数：
 *   @cur — 指向当前 token 的指针（指针的指针，用于消费 token）
 *
 * 返回值：
 *   - 成功：返回构建好的 AST 节点
 *   - 失败：返回 NULL（分配 node 或解析失败）
 *
 * 逻辑：
 *   1. 查看当前 token。
 *   2. 分配 AST 节点 node。
 *      - 分配失败直接返回 NULL。
 *   3. 判断当前 token：
 *      - 如果是 '(' → 调用 parse_subshell 构建子 shell AST。
 *      - 否则 → 调用 parse_normal_cmd_redir_list 构建普通命令 AST。
 */
ast *parse_simple_cmd_redir_list(t_lexer **cur, t_minishell *minishell)
{
    ast *node;
    t_lexer *pt;

    pt = peek_token(cur);
    node = ft_calloc(1, sizeof(ast));
    if (!node)
        return (NULL);
    if (pt && pt->tokentype == TOK_LPAREN)
        return (parse_subshell(cur, node, minishell));
    return (parse_normal_cmd_redir_list(cur, node, minishell));
}
