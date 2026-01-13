/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 23:25:54 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/13 13:27:53 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

void	wait_all_and_set_lt_ast(t_minishell *msh, pid_t *pids, int n)
{
	int		i;
	int		st;
	pid_t	pid;

	i = 0;
	while (i < n)
	{
		pid = waitpid(pids[i], &st, 0);
		if (pid > 0)
		{
			if (WIFSIGNALED(st))
			{
				if (WTERMSIG(st) == SIGPIPE)
				{
				}
				else if (WTERMSIG(st) == SIGSEGV)
					write(STDERR_FILENO, "Segmentation fault\n", 19);
				else if (WTERMSIG(st) == SIGQUIT)
					write(STDERR_FILENO, "Quit: 3\n", 8);
			}
		}
		if (pid == pids[n - 1])
			set_status_from_wait(msh, st);
		i++;
	}
}

int	pipe_make(int i, int n, int out_fd, int pfd[2])
{
	int	child_out;

	pfd[0] = -1;
	pfd[1] = -1;
	if (i < n - 1)
	{
		if (pipe(pfd) < 0)
			return (-1);
		child_out = pfd[1];
	}
	else
		child_out = out_fd;
	return (child_out);
}

void	handle_parent_pipes(t_pipe_ctx *ctx, int pfd[2])
{
	if (ctx->in_fd > STDERR_FILENO)
		close(ctx->in_fd);
	if (pfd[1] != -1)
		close(pfd[1]);
	ctx->in_fd = pfd[0];
}
