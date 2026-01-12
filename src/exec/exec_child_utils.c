/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 22:57:11 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:01:06 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

int	path_is_dir(const char *path)
{
	struct stat	st;

	if (!path)
		return (0);
	if (stat(path, &st) != 0)
		return (0);
	if (S_ISDIR(st.st_mode))
		return (1);
	return (0);
}

int	has_bad_heredoc(t_redir *r)
{
	while (r)
	{
		if (r->type == HEREDOC && r->heredoc_fd < 0)
			return (1);
		r = r->next;
	}
	return (0);
}

static void	close_heredoc_fds(t_redir *r)
{
	while (r)
	{
		if (r->type == HEREDOC && r->heredoc_fd >= 0)
		{
			close(r->heredoc_fd);
			r->heredoc_fd = -1;
		}
		r = r->next;
	}
}

void	close_all_heredoc_fds(t_ast *node)
{
	if (!node)
		return ;
	if (node->type == NODE_CMD)
		close_heredoc_fds(node->redir);
	if (node->sub)
		close_all_heredoc_fds(node->sub);
	if (node->left)
		close_all_heredoc_fds(node->left);
	if (node->right)
		close_all_heredoc_fds(node->right);
}
