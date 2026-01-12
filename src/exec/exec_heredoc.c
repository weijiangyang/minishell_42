/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:21:10 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:21:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "exec.h"

int	prepare_heredocs(t_ast *node, t_minishell *ms)
{
	t_redir	*r;

	if (!node)
		return (1);
	r = node->redir;
	while (r)
	{
		if (r->type == HEREDOC)
		{
			if (handle_heredoc(r, ms) == -1)
				return (0);
		}
		r = r->next;
	}
	if (!prepare_heredocs(node->left, ms))
		return (0);
	if (!prepare_heredocs(node->right, ms))
		return (0);
	return (1);
}
