/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_clear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 18:03:40 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:23:21 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

static void	free_lexer_content(t_lexer *node)
{
	if (!node)
		return ;
	if (node->raw && node->raw == node->str)
	{
		free(node->raw);
		node->raw = NULL;
		node->str = NULL;
	}
	else
	{
		if (node->raw)
		{
			free(node->raw);
			node->raw = NULL;
		}
		if (node->str)
		{
			free(node->str);
			node->str = NULL;
		}
	}
}

t_lexer	*clear_one(t_lexer **lst)
{
	t_lexer	*node;

	if (!lst || !*lst)
		return (NULL);
	node = *lst;
	free_lexer_content(node);
	node->next = NULL;
	node->prev = NULL;
	free(node);
	*lst = NULL;
	return (NULL);
}

void	clear_list(t_lexer **lst)
{
	t_lexer	*tmp;

	if (!lst || !*lst)
		return ;
	while (*lst)
	{
		tmp = (*lst)->next;
		clear_one(lst);
		*lst = tmp;
	}
}
