/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:44 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"
#include "parse.h"

static void	heredoc_fork_child(t_redir *redir, t_minishell *shell,
							int pipefd[2])
{
	int	result;

	setup_heredoc_signals();
	close(pipefd[0]);
	if (redir->quoted)
		result = heredoc_loop(pipefd[1], redir->filename, shell, 1);
	else
		result = heredoc_loop(pipefd[1], redir->filename, shell, 0);
	close(pipefd[1]);
	if (result < 0)
		ms_child_exit(shell, shell->cur_ast, 130);
	ms_child_exit(shell, shell->cur_ast, 0);
}

static int	wait_for_child(pid_t pid, int *status)
{
	while (waitpid(pid, status, 0) == -1)
	{
		if (errno != EINTR)
			return (-1);
	}
	return (0);
}

static int	handle_child_exit(int status, int pipefd[2], t_redir *redir,
							t_minishell *shell)
{
	int	code;

	if (WIFEXITED(status))
	{
		code = WEXITSTATUS(status);
		if (code == 0)
		{
			redir->heredoc_fd = pipefd[0];
			return (0);
		}
		close(pipefd[0]);
		redir->heredoc_fd = -1;
		shell->last_exit_status = code;
		return (-1);
	}
	if (WIFSIGNALED(status))
	{
		close(pipefd[0]);
		redir->heredoc_fd = -1;
		shell->last_exit_status = 128 + WTERMSIG(status);
		return (-1);
	}
	redir->heredoc_fd = pipefd[0];
	return (0);
}

static int	heredoc_parent(pid_t pid, int pipefd[2], t_redir *redir,
					t_minishell *shell)
{
	int	status;

	close(pipefd[1]);
	ignore_heredoc_signals();
	if (wait_for_child(pid, &status) == -1)
		return (-1);
	return (handle_child_exit(status, pipefd, redir, shell));
}

int	handle_heredoc(t_redir *redir, t_minishell *shell)
{
	int				pipefd[2];
	pid_t			pid;
	t_saved_signals	saved;
	int				ret;

	save_signals(&saved);
	if (pipe(pipefd) < 0)
	{
		restore_signals(&saved);
		return (-1);
	}
	pid = fork();
	if (pid < 0)
	{
		restore_signals(&saved);
		return (-1);
	}
	if (pid == 0)
		heredoc_fork_child(redir, shell, pipefd);
	ret = heredoc_parent(pid, pipefd, redir, shell);
	restore_signals(&saved);
	return (ret);
}
