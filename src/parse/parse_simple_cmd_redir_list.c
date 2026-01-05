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
 * @brief 解析简单命令单元或子 Shell 结构。
 * * 该函数是解析器中的决策点：
 * 1. 预读当前 Token (peek_token) 以判断语法分支。
 * 2. 分配一个新的 AST 节点内存。
 * 3. 路径 A：如果检测到左括号 `(`，进入 parse_subshell 处理嵌套的命令流。
 * 4. 路径 B：如果是普通单词或重定向符，进入 parse_normal_cmd_redir_list 解析命令名、参数及重定向。
 * * @param cur        指向当前词法 Token 流指针的地址。
 * @param minishell  指向全局上下文结构体。
 * @return ast* 返回构建好的命令节点或子 Shell 节点；分配失败或解析错误返回 NULL。
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
