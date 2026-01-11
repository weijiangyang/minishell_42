/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 23:25:54 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:26:16 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

/*
** 函数作用：父进程像个班主任一样，等待所有同学（子进程）考试结束。
** 1. 遍历所有子进程 PID，用 waitpid 等待它们。
** 2. 如果有进程因为 broken pipe 挂了，打印提示（模拟 Bash 行为）。
** 3. 重点记录“最后那个命令”的成绩（退出状态码），因为管道的返回值看最后一个。
** @param msh: 全局控制结构体。
** @param pids: 存放所有子进程 PID 的数组。
** @param n: 子进程的总数。
*/
void	wait_all_and_set_lt_ast(t_minishell *msh, pid_t *pids, int n)
{
	int		i;
	int		st;
	int		saw_pipe;
	pid_t	pid;

	i = 0;
	saw_pipe = 0;
	while (i < n)
	{
		pid = waitpid(pids[i], &st, 0);
		if (pid > 0 && pid != pids[n - 1] && saw_pipe == 0)
		{
			if (WIFSIGNALED(st) && WTERMSIG(st) == SIGPIPE)
			{
				write(2, "Broken pipe\n", 12);
				saw_pipe = 1;
			}
		}
		if (pid == pids[n - 1])
			set_status_from_wait(msh, st);
		i++;
	}
}

/*
** 函数作用：决定当前的命令该把结果写到哪里。
** 1. 如果不是最后一个命令，就需要建立管道（pipe），结果写进管道入口 pfd[1]。
** 2. 如果是最后一个命令，直接写到最原本的输出口（out_fd，通常是屏幕或文件）。
** @param i: 当前是第几个命令。
** @param n: 总共有几个命令。
** @param out_fd: 整个管道链条最终的输出口。
** @param pfd: 用来返回新创建的管道文件描述符。
** @return: 返回子进程应该使用的输出 fd，失败返回 -1。
*/
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

/*
** 函数作用：父进程在生完孩子后，关闭不需要的文件描述符。
** 1. 关闭传给当前孩子的输入口（因为它已经被孩子继承并使用了）。
** 2. 关闭当前管道的写入口（父进程不写，只有孩子写）。
** 3. 把当前管道的读出口（pfd[0]）保存下来，留给下一个孩子当输入用。
** @param ctx: 管道执行上下文结构体。
** @param pfd: 当前创建的管道数组。
*/
void	handle_parent_pipes(t_pipe_ctx *ctx, int pfd[2])
{
	if (ctx->in_fd > 2)
		close(ctx->in_fd);
	if (pfd[1] != -1)
		close(pfd[1]);
	ctx->in_fd = pfd[0];
}
