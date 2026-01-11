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

/*
** 函数作用：
** 只有重定向、没有命令时（如：> a 或 < infile）：
** - 仍然要按顺序“尝试应用重定向”，从而创建文件/检查文件并在失败时报错。
** - 不执行任何命令；成功返回 0，失败返回 1（更贴近 minishell 行为）。
** - 同时把传进来的 in_fd/out_fd（若非标准 fd）关闭，避免父进程泄露。
*/
static int	run_redir_only_parent(t_minishell *msh, t_ast *node, int in_fd,
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
** 这个函数是父进程专用的收尾工作，就像家长在校门口接孩子。
** 1. 关掉不再需要的管道文件（防止资源泄露）。
** 2. 专心等待刚才生出来的子进程（pid）放学（运行结束）。
** 3. 拿到子进程的成绩单（退出状态 st），更新到全局记录里。
** 4. 整理好心情（恢复信号），准备迎接用户的下一次输入。
** @param pid: 子进程的身份证号。
** @param in/out_fd: 要关闭的文件描述符。
*/
static int	handle_parent_wait(t_minishell *msh, pid_t pid, int in_fd,
		int out_fd)
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

/*
** 这个函数负责运行一个外部命令，并且让父进程停下来等它。
** 1. 设置好信号（比如忽略 Ctrl+C，防止父进程被误杀）。
** 2. 尝试分身（fork）：
** - 如果失败（pid < 0）：报错，关灯（关文件），直接回家（返回错误）。
** - 如果是孩子（pid == 0）：去执行具体的命令（child_exec_one）。
** - 如果是父亲（pid > 0）：调用上面的函数，乖乖等待孩子结束。
*/
static int	run_external_wait(t_minishell *msh, t_ast *node, int in_fd,
		int out_fd)
{
	pid_t	pid;

	setup_parent_exec_signals();
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
	{
		setup_child_signals();
		child_exec_one(msh, node, in_fd, out_fd, node);
	}
	return (handle_parent_wait(msh, pid, in_fd, out_fd));
}

/*
** 函数作用：判断是否必须在父进程执行（会改变父进程状态的 builtin）。
*/
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

/*
** 函数作用：
** 执行一个 CMD 节点：
** 1) heredoc 被 Ctrl+C 打断就直接返回 130，不执行命令
** 2) 如果没有 argv[0]（只有重定向），也要执行重定向副作用/报错
** 3) builtin 在父进程执行
** 4) 外部命令 fork+wait
*/
int	exec_cmd_node(t_minishell *msh, t_ast *node, int in_fd, int out_fd)
{
	int	ret;

	ret = 0;
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
