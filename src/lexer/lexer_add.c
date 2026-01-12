/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_add.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 22:12:34 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:23:07 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

static void	init_node_info(t_lexer *new, t_token_info *info)
{
	new->str = NULL;
	new->raw = NULL;
	new->had_quotes = 0;
	new->quoted_by = 0;
	if (!info)
		return ;
	new->str = info->clean;
	new->raw = info->raw;
	new->had_quotes = info->had_quotes;
	if (info->quoted_single)
		new->quoted_by += 1;
	if (info->quoted_double)
		new->quoted_by += 2;
}

static int	assign_node_name(t_lexer *new, t_tok_type type)
{
	const char	*symbol;

	symbol = token_symbol(type);
	if (!symbol)
		return (1);
	new->raw = ft_strdup(symbol);
	if (!new->raw)
		return (0);
	new->str = ft_strdup(symbol);
	if (!new->str)
	{
		free(new->raw);
		return (0);
	}
	return (1);
}

t_lexer	*new_node(t_token_info *info, t_tok_type type)
{
	t_lexer		*new;
	static int	count = 0;

	new = malloc(sizeof(*new));
	if (!new)
		return (NULL);
	ft_bzero(new, sizeof(*new));
	init_node_info(new, info);
	if (info == NULL)
	{
		if (!assign_node_name(new, type))
		{
			free(new);
			return (NULL);
		}
	}
	new->tokentype = type;
	new->idx = count++;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}

void	list_add_back(t_lexer **lst, t_lexer *new)
{
	t_lexer	*last;

	if (!lst || !new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next)
		last = last->next;
	last->next = new;
	new->prev = last;
}

int	add_node(t_token_info *info, t_tok_type tokentype, t_lexer **list)
{
	t_lexer	*node;

	node = new_node(info, tokentype);
	if (!node)
		return (0);
	list_add_back(list, node);
	return (1);
}
