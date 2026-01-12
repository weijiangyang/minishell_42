/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:25:55 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/30 06:41:55 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

static void	close_keep_std(int *fd)
{
	if (!fd)
		return ;
	if (*fd > STDERR_FILENO)
	{
		close(*fd);
		*fd = -1;
	}
}

static int	open_redir_fd(t_redir *r)
{
	int	fd;

	fd = -1;
	if (r->type == REDIR_INPUT)
		fd = open(r->filename, O_RDONLY);
	else if (r->type == REDIR_OUTPUT)
		fd = open(r->filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else if (r->type == REDIR_APPEND)
		fd = open(r->filename, O_CREAT | O_WRONLY | O_APPEND, 0644);
	else if (r->type == HEREDOC && r->heredoc_fd >= 0)
		fd = dup(r->heredoc_fd);
	return (fd);
}

int	apply_redir_list(t_redir *r, int *in_fd, int *out_fd)
{
	int	fd;

	while (r)
	{
		if (r->type == REDIR_INPUT || r->type == HEREDOC)
			close_keep_std(in_fd);
		else
			close_keep_std(out_fd);
		fd = open_redir_fd(r);
		if (fd < 0)
		{
			if (r->filename)
				ms_err_redir(r->filename, errno);
			else
				ms_err_redir("redir", errno);
			return (close_keep_std(in_fd), close_keep_std(out_fd), -1);
		}
		if (r->type == REDIR_INPUT || r->type == HEREDOC)
			*in_fd = fd;
		else
			*out_fd = fd;
		r = r->next;
	}
	return (0);
}
