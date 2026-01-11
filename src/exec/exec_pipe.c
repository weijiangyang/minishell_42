/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:27:46 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:39:05 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

/*
** 函数作用：管道中子进程的具体执行逻辑。
** 1. 拿到属于自己的命令节点。
** 2. 关掉管道读取端（因为子进程只负责写，不负责读这个新管道）。
** 3. 释放父进程用来记录 PID 的数组内存（子进程不需要）。
** 4. 调用 child_exec_one 去执行具体命令。
** @param ctx: 上下文结构体。
** @param i: 当前索引。
** @param out_fd: 确定的输出目标。
** @param pfd: 管道文件描述符数组。
*/
static void	exec_pipe_child(t_pipe_ctx *ctx, int i, int out_fd, int pfd[2])
{
	t_ast	*cmd;
	int		in_fd;

	cmd = ctx->arr[i];
	in_fd = ctx->in_fd;
	if (pfd[0] != -1)
		close(pfd[0]);
	free(ctx->pids);
	free(ctx->arr);
	child_exec_one(ctx->msh, cmd, in_fd, out_fd, ctx->root);
}

/*
** 函数作用：处理管道链条中的“这一步”。
** 1. 准备管道（决定输出去哪）。
** 2. 分身（Fork）。
** 3. 如果是孩子：去执行 exec_pipe_child。
** 4. 如果是父亲：做清理工作，准备下一步。
** @param ctx: 上下文结构体。
** @param i: 当前步骤索引。
** @return: 成功返回 1，失败返回 0。
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
		exec_pipe_child(ctx, i, child_out, pfd);
	ctx->pids[i] = pid;
	handle_parent_pipes(ctx, pfd);
	return (1);
}

/*
** 函数作用：一个接一个地启动所有管道命令。
** 比如 ls | grep a | wc -l，这里会循环 3 次，启动 3 个进程。
** @param ctx: 上下文结构体。
** @param done: 指针，用来记录成功启动了多少个进程。
** @return: 全部成功返回 1，中途失败返回 0。
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
		i++;
	}
	*done = i;
	return (ok);
}

/*
** 函数作用：管道执行的总入口。
** 1. 收集所有通过管道连接的命令到数组里。
** 2. 分配内存用来存 PID。
** 3. 启动所有进程（pipe_run_all）。
** 4. 最后等待所有进程结束，回收资源。
** @param msh: 全局结构体。
** @param node: AST 当前节点（PIPE节点）。
** @param in_fd/out_fd: 整体的输入输出环境。
*/
int	exec_pipe_node(t_minishell *msh, t_ast *node, int in_fd, int out_fd)
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
	ctx.root = node;
	ctx.in_fd = in_fd;
	ctx.out_fd = out_fd;
	ok = pipe_run_all(&ctx, &done);
	if (ctx.in_fd > STDERR_FILENO)
		close(ctx.in_fd);
	wait_all_and_set_lt_ast(msh, ctx.pids, done);
	if (!ok)
		msh->last_exit_status = 1;
	return (free(ctx.pids), free(ctx.arr), msh->last_exit_status);
}
