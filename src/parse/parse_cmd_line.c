/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yang <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 22:19:34 by yang              #+#    #+#             */
/*   Updated: 2025/10/31 22:19:38 by yang             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

/**
 * @brief 解析整行命令行输入并构建抽象语法树 (AST)。
 * * 该函数作为解析器的总入口，执行以下逻辑：
 * 1. 调用 parse_pipeline 启动递归下降解析（处理命令和管道）。
 * 2. 解析完成后，使用 peek_token 检查词法流中是否还有多余的 Token。
 * 3. 如果解析结束但仍存在非结束符 (TOK_END) 的 Token，说明存在语法错误（如多余的闭括号或未识别字符）。
 * * @param cur        指向当前词法 Token 流指针的地址。
 * @param minishell  指向全局上下文结构体，用于解析过程中的状态记录。
 * @return ast* 成功则返回构建好的 AST 根节点；发生语法错误则释放已分配内存并返回 NULL。
 */
ast *parse_cmdline(t_lexer **cur, t_minishell *minishell)
{
    ast *root;
    t_lexer *pt;

    root = parse_pipeline(cur, minishell);
    if (!root)
        return NULL;
    pt = peek_token(cur);
    if (pt && pt->tokentype != TOK_END)
    {
        minishell->parse_status = PARSE_SYNTAX_ERROR;
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n", pt->tokentype);
        free_ast(root);
        return NULL;
    }
    return root;
}
