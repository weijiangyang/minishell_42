/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_common.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 19:07:52 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:02:33 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

int	dup_in_out_or_close(int in_fd, int out_fd)
{
	if (in_fd >= 0 && in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) < 0)
		{
			close(in_fd);
			if (out_fd >= 0 && out_fd != STDOUT_FILENO)
				close(out_fd);
			return (-1);
		}
		close(in_fd);
	}
	if (out_fd >= 0 && out_fd != STDOUT_FILENO)
	{
		if (dup2(out_fd, STDOUT_FILENO) < 0)
		{
			close(out_fd);
			return (-1);
		}
		close(out_fd);
	}
	return (0);
}

int	save_std_fds(t_fd_save *save)
{
	if (!save)
		return (1);
	save->in = dup(STDIN_FILENO);
	save->out = dup(STDOUT_FILENO);
	if (save->in < 0 || save->out < 0)
		return (1);
	return (0);
}

void	restore_std_fds(t_fd_save *save)
{
	if (!save)
		return ;
	if (save->in >= 0)
	{
		dup2(save->in, STDIN_FILENO);
		close(save->in);
	}
	if (save->out >= 0)
	{
		dup2(save->out, STDOUT_FILENO);
		close(save->out);
	}
}

void	set_status_from_wait(t_minishell *msh, int status)
{
	int	sig;

	if (WIFEXITED(status))
		msh->last_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGQUIT)
			write(1, "Quit (core dumped)\n", 19);
		else if (sig == SIGINT)
			write(1, "\n", 1);
		msh->last_exit_status = 128 + sig;
	}
	else if (WIFSTOPPED(status))
	{
		write(1, "\n", 1);
		ft_putstr_fd("[1]+  Stopped\n", 1);
		msh->last_exit_status = 128 + WSTOPSIG(status);
	}
}

int	wait_pair_set_right(t_minishell *msh, pid_t left, pid_t right)
{
	int	st;

	st = 0;
	if (left > 0)
		waitpid(left, NULL, 0);
	if (right > 0)
	{
		if (waitpid(right, &st, 0) > 0)
			set_status_from_wait(msh, st);
		else
			msh->last_exit_status = 1;
	}
	return (msh->last_exit_status);
}
