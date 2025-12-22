/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 23:59:59 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:34:57 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "../../include/minishell.h"
#include "error.h"

/*
** 函数作用：
** 检查这个命令的重定向里有没有“坏掉的 heredoc”：
** 如果 heredoc 在解析阶段被 Ctrl+C 打断，fd 会变成 -1。
** 这种情况下执行器不要再跑命令，直接当作被中断。
*/
static int	has_bad_heredoc(t_redir *r)
{
	while (r)
	{
		if (r->type == HEREDOC && r->heredoc_fd < 0)
			return (1);
		r = r->next;
	}
	return (0);
}

/*
** 函数作用：
** 只有重定向、没有命令时（如：> a 或 < infile）：
** - 仍然要按顺序“尝试应用重定向”，从而创建文件/检查文件并在失败时报错。
** - 不执行任何命令；成功返回 0，失败返回 1（更贴近 bash 行为）。
** - 同时把传进来的 in_fd/out_fd（若非标准 fd）关闭，避免父进程泄露。
*/
static int	run_redir_only_parent(t_minishell *msh, ast *node, int in_fd,
		int out_fd)
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
	if (in_fd > STDERR_FILENO)
		close(in_fd);
	if (out_fd > STDERR_FILENO)
		close(out_fd);
	msh->last_exit_status = ret;
	return (ret);
}

/*
** 函数作用：
** 让父进程执行 builtin（比如 cd/export/unset/exit 才会真正生效）。
** 同时支持重定向：先保存标准输入输出 -> 应用重定向 -> 跑 builtin -> 恢复。
** 跑完后同步 envp 和 PATH 缓存，保证后续外部命令能用新环境。
*/
static int	run_builtin_parent(t_minishell *msh, ast *node, int in_fd,
		int out_fd)
{
	t_fd_save	save;
	int			new_in;
	int			new_out;
	int			ret;

	new_in = STDIN_FILENO;
	new_out = STDOUT_FILENO;
	ret = 1;
	if (save_std_fds(&save) != 0)
	{
		if (in_fd >= 0 && in_fd != STDIN_FILENO)
			close(in_fd);
		if (out_fd >= 0 && out_fd != STDOUT_FILENO)
			close(out_fd);
		return (1);
	}
	if (dup_in_out_or_close(in_fd, out_fd) < 0)
		return (restore_std_fds(&save), 1);
	if (apply_redir_list(node->redir, &new_in, &new_out) < 0)
		return (restore_std_fds(&save), 1);
	if (dup_in_out_or_close(new_in, new_out) < 0)
		return (restore_std_fds(&save), 1);
	ret = exec_builtin(node, &msh->env);
	change_envp(msh->env, &msh->envp);
	exec_refresh_paths(msh);
	restore_std_fds(&save);
	return (ret);
}

/*
** 函数作用：
** 执行外部命令：fork 出子进程去 execve，父进程负责关闭 fd 和 wait。
** wait 结束后，把子进程的退出码写回 msh->last_exit_status。
** 关键修复：
** - fork() 失败时，关闭传进来的 in_fd/out_fd（非 STD），避免父进程泄露。
*/
static int	run_external_wait(t_minishell *msh, ast *node, int in_fd,
		int out_fd)
{
	pid_t	pid;
	int		st;

	st = 0;
	pid = fork();
	if (pid < 0)
	{
		ms_perror("fork");
		if (in_fd > STDERR_FILENO)
			close(in_fd);
		if (out_fd > STDERR_FILENO)
			close(out_fd);
		msh->last_exit_status = 1;
		return (1);
	}
	if (pid == 0)
		child_exec_one(msh, node, in_fd, out_fd);
	if (in_fd > STDERR_FILENO)
		close(in_fd);
	if (out_fd > STDERR_FILENO)
		close(out_fd);
	if (waitpid(pid, &st, 0) > 0)
		set_status_from_wait(msh, st);
	return (msh->last_exit_status);
}

/*
** 函数作用：
** 执行一个 CMD 节点：
** 1) heredoc 被 Ctrl+C 打断就直接返回 130，不执行命令
** 2) 如果没有 argv[0]（只有重定向），也要执行重定向副作用/报错
** 3) builtin 在父进程执行
** 4) 外部命令 fork+wait
*/
int	exec_cmd_node(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	int	ret;

	ret = 0;
	if (!node)
		return (0);
	if (has_bad_heredoc(node->redir))
	{
		ret = msh->last_exit_status;
		if (ret == 0)
			ret = 130;
		msh->last_exit_status = ret;
		return (ret);
	}
	if (!node->argv || !node->argv[0])
		return (run_redir_only_parent(msh, node, in_fd, out_fd));
	if (is_builtin(node->argv[0]))
	{
		ret = run_builtin_parent(msh, node, in_fd, out_fd);
		msh->last_exit_status = ret;
		return (ret);
	}
	ensure_paths_ready(msh);
	return (run_external_wait(msh, node, in_fd, out_fd));
}
