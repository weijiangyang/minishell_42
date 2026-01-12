/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_t_ast_tokens.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:28:05 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:28:18 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

/**
 * @brief 释放 t_ast 节点及其关联的重定向链表和参数数组。
 * * 该函数执行以下清理操作：
 * 1. 遍历释放重定向链表 (node->redir)。
 * 2. 遍历并释放参数数组 (node->argv) 中的每个字符串。
 * 3. 释放参数数组本身的指针。
 * 4. 释放节点结构体本身的内存。
 * * @param node 指向需要释放的 t_ast 节点的指针。如果为 NULL 则直接返回。
 */
void free_ast_partial(t_ast *node)
{
	int i;

	i = 0;
	if (!node)
		return;
	if (node->redir)
		free_redir_list(node->redir);
	if (node->argv)
	{
		while (node->argv[i])
		{
			free(node->argv[i]);
			i++;
		}
		free(node->argv);
	}
	free(node);
}

/**
 * @brief 递归释放整个抽象语法树（t_ast）。
 * * 根据节点的类型（NODE_CMD, NODE_PIPE, NODE_SUBSHELL）采取不同的清理策略：
 * 1. NODE_CMD: 叶子节点，调用 free_t_ast_partial 释放关联参数和重定向。
 * 2. NODE_PIPE: 中间节点，递归释放左子树（左侧命令）和右子树（右侧命令）。
 * 3. NODE_SUBSHELL: 递归进入子 Shell 内部的抽象语法树。
 * * @param node 指向 t_ast 根节点或子树节点的指针。
 */
void free_ast(t_ast *node)
{
	if (!node)
		return;
	if (node->type == NODE_CMD)
	{
		free_ast_partial(node);
		return;
	}
	else if (node->type == NODE_PIPE)
	{
		if (node->left)
			free_ast(node->left);
		if (node->right)
			free_ast(node->right);
	}
	else if (node->type == NODE_SUBSHELL)
		free_ast(node->sub);
	free(node);
}

/**
 * @brief 释放 Token 链表及其关联的所有内存。
 * * 遍历词法分析生成的链表，并释放：
 * 1. 处理后的字符串副本 (tok->str)，通常用于环境变量展开后的内容。
 * 2. 原始字符串副本 (tok->raw)，通常用于保留未处理的原始输入。
 * 3. Token 结构体本身的内存。
 * * @param tok 指向待释放 Token 链表头部的指针。
 */
void	free_tokens(t_lexer *tok)
{
	t_lexer	*next;

	while (tok)
	{
		next = tok->next;
		if (tok->raw)
			free(tok->raw);
		if (tok->str && tok->str != tok->raw)
			free(tok->str);
		free(tok);
		tok = next;
	}
}

