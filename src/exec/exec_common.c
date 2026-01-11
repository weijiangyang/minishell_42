/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_common.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 19:07:52 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

/*
** 函数作用：
** 把 in_fd 接到标准输入，把 out_fd 接到标准输出。
** 如果传进来的 fd 不是标准 fd，就 dup2 后把原 fd 关闭，防止泄漏。
** 参数：
** in_fd：要作为 stdin 的 fd（>=0 才处理）
** out_fd：要作为 stdout 的 fd（>=0 才处理）
** 返回：
** 成功返回 0；失败返回 -1
*/
int dup_in_out_or_close(int in_fd, int out_fd)
{
	if (in_fd >= 0 && in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) < 0)
		{
			close(in_fd);
			if (out_fd >= 0 && out_fd != STDOUT_FILENO)
				close(out_fd); /* 修改：避免 out_fd 泄漏 */
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

/*
** 函数作用：
** 保存当前标准输入输出，给“父进程跑 builtin 且带重定向”用。
*/
int save_std_fds(t_fd_save *save)
{
	if (!save)
		return (1);
	save->in = dup(STDIN_FILENO);
	save->out = dup(STDOUT_FILENO);
	if (save->in < 0 || save->out < 0)
		return (1);
	return (0);
}
/*
** 函数作用：
** 恢复之前保存的标准输入输出。
** 让重定向只影响一次 builtin，不影响后面的提示符。
*/
void restore_std_fds(t_fd_save *save)
{
	if (!save)
		return;
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
/*
** 函数作用：
** 把 waitpid 的返回状态 st 转换成 shell 退出码：
** 正常 exit -> 取 exit code；被信号杀死 -> 128 + 信号号。
*/
void set_status_from_wait(t_minishell *msh, int status)
{
	if (WIFEXITED(status))
	{
		msh->last_exit_status = WEXITSTATUS(status);
	}
	else if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		if (sig == SIGQUIT)
			write(1, "Quit (core dumped)\n", 19);
		else if (sig == SIGINT)
			write(1, "\n", 1);
		msh->last_exit_status = 128 + sig;
	}
	else if (WIFSTOPPED(status))
	{
		// 当按下 Ctrl+Z 时，子进程停止，waitpid(..., WUNTRACED) 返回
		// 1. 打印一个换行，避免提示符和 ^Z 挤在一起
		write(1, "\n", 1);
		// 2. 告诉用户进程已停止（模拟真实 Shell）
		printf("[1]+  Stopped\n");
		msh->last_exit_status = 128 + WSTOPSIG(status);
	}
}
/*
** 函数作用：
** 等待一对管道子进程，并把“右边命令”的退出码当作整条管道的退出码。
*/
int wait_pair_set_right(t_minishell *msh, pid_t left, pid_t right)
{
	int st;

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
