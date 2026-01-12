/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 23:59:59 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:22:51 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

static int	run_redir_only_parent(t_minishell *msh, t_ast *node,
	int in, int out)
{
	int	new_in;
	int	new_out;
	int	ret;

	new_in = STDIN_FILENO;
	new_out = STDOUT_FILENO;
	ret = 0;
	if (apply_redir_list(node->redir, &new_in, &new_out) < 0)
		ret = 1;
	if (new_in > STDERR_FILENO)
		close(new_in);
	if (new_out > STDERR_FILENO)
		close(new_out);
	if (in > STDERR_FILENO)
		close(in);
	if (out > STDERR_FILENO)
		close(out);
	msh->last_exit_status = ret;
	return (ret);
}

static int	handle_parent_wait(t_minishell *msh, pid_t pid,
	int in_fd, int out_fd)
{
	int	st;

	if (in_fd > STDERR_FILENO)
		close(in_fd);
	if (out_fd > STDERR_FILENO)
		close(out_fd);
	st = 0;
	if (waitpid(pid, &st, WUNTRACED) > 0)
		set_status_from_wait(msh, st);
	setup_prompt_signals();
	rl_on_new_line();
	return (msh->last_exit_status);
}

static int	run_external_wait(t_minishell *msh, t_ast *node, int in, int out)
{
	pid_t		pid;
	t_exec_ctx	ctx;

	setup_parent_exec_signals();
	pid = fork();
	if (pid < 0)
	{
		ms_perror("fork");
		if (in > STDERR_FILENO)
			close(in);
		if (out > STDERR_FILENO)
			close(out);
		msh->last_exit_status = 1;
		return (1);
	}
	if (pid == 0)
	{
		setup_child_signals();
		ctx = (t_exec_ctx){msh, node, node, in, out};
		child_exec_one(&ctx);
	}
	return (handle_parent_wait(msh, pid, in, out));
}

static int	is_builtin_parent(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	return (0);
}

int	exec_cmd_node(t_minishell *msh, t_ast *node, int in_fd, int out_fd)
{
	int	ret;

	if (!node)
		return (0);
	if (has_bad_heredoc(node->redir))
	{
		ret = msh->last_exit_status;
		return (ret);
	}
	if (!node->argv || !node->argv[0])
		return (run_redir_only_parent(msh, node, in_fd, out_fd));
	if (is_builtin_parent(node->argv[0]))
	{
		ret = run_builtin_parent_logic(msh, node, in_fd, out_fd);
		msh->last_exit_status = ret;
		return (ret);
	}
	ensure_paths_ready(msh);
	return (run_external_wait(msh, node, in_fd, out_fd));
}
