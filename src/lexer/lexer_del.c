/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_del.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:19:05 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:05:48 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"

void	del_first(t_lexer **lst)
{
	t_lexer	*tmp;

	if (!lst || !*lst)
		return ;
	tmp = *lst;
	*lst = tmp->next;
	clear_one(&tmp);
	if (*lst)
		(*lst)->prev = NULL;
}

static void	remove_target(t_lexer *start, int target)
{
	t_lexer	*prev;
	t_lexer	*node;

	prev = start;
	node = start->next;
	while (node && node->idx != target)
	{
		prev = node;
		node = node->next;
	}
	if (!node)
		return ;
	prev->next = node->next;
	if (node->next)
		node->next->prev = prev;
	clear_one(&node);
}

void	del_one(t_lexer **lst, int target)
{
	t_lexer	*start;

	if (!lst || !*lst)
		return ;
	start = *lst;
	if (start->idx == target)
	{
		del_first(lst);
		return ;
	}
	remove_target(start, target);
}
