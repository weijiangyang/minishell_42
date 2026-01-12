/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_t_ast.c                                        :+:      :+:    :+: */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 22:37:06 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:37:12 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"

static int	walk_t_ast(t_minishell *msh, t_ast *node)
{
	if (!node)
		return (1);
	if (node->type == NODE_CMD)
	{
		if (!expander_expand_cmd_node(msh, node))
			return (0);
	}
	if (node->left && !walk_t_ast(msh, node->left))
		return (0);
	if (node->right && !walk_t_ast(msh, node->right))
		return (0);
	if (node->sub && !walk_t_ast(msh, node->sub))
		return (0);
	return (1);
}

int	expander_t_ast(t_minishell *minishell, t_ast *root)
{
	if (!minishell)
		return (0);
	return (walk_t_ast(minishell, root));
}
