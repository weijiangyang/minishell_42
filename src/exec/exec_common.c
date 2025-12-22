/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_common.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 19:07:52 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:37:44 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/exec.h"

/*
** 函数作用：
** 把 in_fd 接到标准输入，把 out_fd 接到标准输出。
** 如果传进来的 fd 不是标准 fd，就 dup2 后把原 fd 关闭，防止泄漏。
*/
int	dup_in_out_or_close(int in_fd, int out_fd)
{
	if (in_fd >= 0 && in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) < 0)
		{
			close(in_fd);
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

/*
** 函数作用：
** 恢复之前保存的标准输入输出。
** 让重定向只影响一次 builtin，不影响后面的提示符。
*/
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

/*
** 函数作用：
** 把 waitpid 的返回状态 st 转换成 shell 退出码：
** 正常 exit -> 取 exit code；被信号杀死 -> 128 + 信号号。
*/
void	set_status_from_wait(t_minishell *msh, int st)
{
	if (!msh)
		return ;
	if (WIFEXITED(st))
		msh->last_exit_status = WEXITSTATUS(st);
	else if (WIFSIGNALED(st))
		msh->last_exit_status = 128 + WTERMSIG(st);
	else
		msh->last_exit_status = 1;
}

/*
** 函数作用：
** 等待一对管道子进程，并把“右边命令”的退出码当作整条管道的退出码。
*/
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
