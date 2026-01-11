/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_subshell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:30:13 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:30:14 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

/**
 * @brief 解析子 Shell 结构 ( (command) )。
 * * 该函数处理括号包围的命令流：
 * 1. 消耗左括号 `(`。
 * 2. 设置节点类型为 NODE_SUBSHELL。
 * 3. 递归调用 parse_pipeline 解析括号内部的完整管道线，并挂载到 node->sub。
 * 4. 验证并消耗右括号 `)`。如果缺少右括号，则报错并清理已分配的 AST 内存。
 * * @param cur        指向当前词法 Token 流指针的地址。
 * @param node       当前正在构建的 AST 节点。
 * @param minishell  指向全局上下文结构体。
 * @return ast* 成功构建子 Shell 节点返回 node；解析内部失败或缺少右括号返回 NULL。
 */
ast *parse_subshell(t_lexer **cur, ast *node, t_minishell *minishell)
{
    consume_token(cur);
    node->type = NODE_SUBSHELL;
    node->sub = parse_pipeline(cur, minishell);
    if (!expect_token(TOK_RPAREN, cur))
    {
        fprintf(stderr, "Syntax error: expected ')'\n");
        free_ast(node);
        return NULL;
    }
    return node;
}
