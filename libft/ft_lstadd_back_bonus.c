/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 15:45:39 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 18:20:12 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list *lt_ast;

	if (!lst || !new)
		return;
	if (!*lst)
	{
		*lst = new;
		return;
	}
	lt_ast = ft_lstlt_ast(*lst);
	lt_ast->next = new;
}
