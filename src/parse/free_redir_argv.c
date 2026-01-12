/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_redir_argv.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 16:25:07 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/25 16:25:09 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

void	free_redir_list(t_redir *r)
{
	t_redir	*next;

	while (r)
	{
		next = r->next;
		if (r->type == HEREDOC && r->heredoc_fd >= 0)
		{
			close(r->heredoc_fd);
			r->heredoc_fd = -1;
		}
		free(r->filename);
		free(r);
		r = next;
	}
}

void	free_argv_list(t_cmd *a)
{
	t_cmd	*next;

	while (a)
	{
		next = a->next;
		free(a->content);
		free(a);
		a = next;
	}
}

void	free_t_cmd_node(t_cmd *argv_cmd)
{
	t_cmd	*tmp;
	t_cmd	*next;

	tmp = argv_cmd;
	while (tmp)
	{
		next = tmp->next;
		free(tmp);
		tmp = next;
	}
}
