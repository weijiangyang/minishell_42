/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_t_ast_tokens.c                                  :+:      :+:    :+: */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:28:05 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:28:18 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

void	free_ast_partial(t_ast *node)
{
	int	i;

	if (!node)
		return ;
	if (node->redir)
		free_redir_list(node->redir);
	if (node->argv)
	{
		i = 0;
		while (node->argv[i])
		{
			free(node->argv[i]);
			i++;
		}
		free(node->argv);
	}
	free(node);
}

void	free_ast(t_ast *node)
{
	if (!node)
		return ;
	if (node->type == NODE_CMD)
	{
		free_ast_partial(node);
		return ;
	}
	if (node->left)
		free_ast(node->left);
	if (node->right)
		free_ast(node->right);
	if (node->sub)
		free_ast(node->sub);
	free(node);
}

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
