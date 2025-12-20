/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 21:03:13 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/20 17:06:49 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

/*
** 函数作用：递归走完整棵 AST，遇到 CMD 节点就调用展开函数。
** 参数：msh(全局上下文), node(AST 当前节点)
** 返回：成功 1，失败 0
*/
static int	expand_dfs(t_minishell *msh, ast *node)
{
	if (!node)
		return (1);
	if (node->type == NODE_CMD)
	{
		if (!expander_expand_cmd_node(msh, node))
			return (0);
	}
	if (node->left && !expand_dfs(msh, node->left))
		return (0);
	if (node->right && !expand_dfs(msh, node->right))
		return (0);
	if (node->sub && !expand_dfs(msh, node->sub))
		return (0);
	return (1);
}

/*
** 函数作用：扩展总入口：parse 完成后调用它。
** 参数：minishell(全局上下文), root(AST 根)
** 返回：成功 1，失败 0
*/
int	expander_ast(t_minishell *minishell, ast *root)
{
	if (!minishell)
		return (0);
	return (expand_dfs(minishell, root));
}
