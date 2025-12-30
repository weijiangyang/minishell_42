/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:27:46 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/30 04:26:39 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "../../include/minishell.h"

/*
** 函数作用：等待所有子进程结束，并把“最后一个命令”的退出码写回 msh。
** 额外行为：如果有非最后一个命令因为 SIGPIPE 结束，打印 " broken pipe"（贴近 bash）。
*/
static void	wait_all_and_set_last(t_minishell *msh, pid_t *pids, int n)
{
	int		i;
	int		st;
	int		saw_pipe;
	pid_t	pid;
	pid_t	last_pid;

	i = 0;
	st = 0;
	saw_pipe = 0;
	last_pid = pids[n - 1];
	while (i < n)
	{
		pid = waitpid(pids[i], &st, 0);
		if (pid > 0 && pid != last_pid && saw_pipe == 0)
		{
			if (WIFSIGNALED(st) && WTERMSIG(st) == SIGPIPE)
				(write(2, "Broken pipe\n", 12), saw_pipe = 1);
		}
		if (pid == last_pid)
			set_status_from_wait(msh, st);
		i = i + 1;
	}
}

/*
** 函数作用：为第 i 段准备 pipe，并返回子进程要写到的 fd。
** 规则：不是最后一段 -> pipe()，子进程写 pfd[1]；最后一段 -> 直接写 out_fd。
** 返回值：child_out，失败返回 -1。
*/
static int	pipe_make(int i, int n, int out_fd, int pfd[2])
{
	int	child_out;

	child_out = -1;
	pfd[0] = -1;
	pfd[1] = -1;
	if (i < n - 1)
	{
		if (pipe(pfd) < 0)
			return (-1);
		child_out = pfd[1];
		return (child_out);
	}
	child_out = out_fd;
	return (child_out);
}

/*
** 函数作用：执行 pipeline 的第 i 段：建管道(如需) -> fork -> 父进程关 fd 并传递 in_fd。
** 返回值：成功 1，失败 0。
*/
static int	pipe_step(t_pipe_ctx *ctx, int i)
{
	int		pfd[2];
	int		child_out;
	pid_t	pid;

	child_out = pipe_make(i, ctx->n, ctx->out_fd, pfd);
	if (child_out < 0)
		return (0);
	pid = fork();
	if (pid < 0)
	{
		if (pfd[0] != -1)
			close(pfd[0]);
		if (pfd[1] != -1)
			close(pfd[1]);
		return (0);
	}
	if (pid == 0)
	{
		if (pfd[0] != -1)
			close(pfd[0]);
		child_exec_one(ctx->msh, ctx->arr[i], ctx->in_fd, child_out);
	}
	ctx->pids[i] = pid;
	if (ctx->in_fd > STDERR_FILENO)
		close(ctx->in_fd);
	if (pfd[1] != -1)
		close(pfd[1]);
	ctx->in_fd = pfd[0];
	return (1);
}

/*
** 函数作用：按顺序启动整条 pipeline（父进程循环 fork 每一段）。
** done 用来告诉外面：一共成功启动了多少个子进程。
*/
static int	pipe_run_all(t_pipe_ctx *ctx, int *done)
{
	int	i;
	int	ok;

	i = 0;
	ok = 1;
	while (i < ctx->n && ok)
	{
		ok = pipe_step(ctx, i);
		i = i + 1;
	}
	*done = i;
	return (ok);
}

/*
** 函数作用：执行任意长度的 pipeline：a | b | c | ...
** 关键点：返回值必须等于“最后一个命令”的退出码（bash 默认行为）。
*/
int	exec_pipe_node(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	t_pipe_ctx	ctx;
	int			done;
	int			ok;

	ft_bzero(&ctx, sizeof(ctx));
	done = 0;
	ok = pipe_collect(node, &ctx.arr, &ctx.n);
	if (!ok)
		return (0);
	ctx.pids = (pid_t *)malloc(sizeof(*ctx.pids) * ctx.n);
	if (!ctx.pids)
		return (free(ctx.arr), 0);
	ctx.msh = msh;
	ctx.in_fd = in_fd;
	ctx.out_fd = out_fd;
	ok = pipe_run_all(&ctx, &done);
	if (ctx.in_fd > STDERR_FILENO)
		close(ctx.in_fd);
	wait_all_and_set_last(msh, ctx.pids, done);
	if (!ok)
		msh->last_exit_status = 1;
	return (free(ctx.pids), free(ctx.arr), msh->last_exit_status);
}
