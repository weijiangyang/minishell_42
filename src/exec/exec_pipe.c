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

static void	exec_pipe_child(t_pipe_ctx *ctx, int i, int out_fd, int pfd[2])
{
	t_ast		*cmd;
	int			in_fd;
	t_exec_ctx	ctx_exec;

	cmd = ctx->arr[i];
	in_fd = ctx->in_fd;
	if (pfd[0] != -1)
		close(pfd[0]);
	free(ctx->pids);
	free(ctx->arr);
	ctx_exec.msh = ctx->msh;
	ctx_exec.node = cmd;
	ctx_exec.in_fd = in_fd;
	ctx_exec.out_fd = out_fd;
	ctx_exec.root = ctx->root;
	child_exec_one(&ctx_exec);
}

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

static int	pipe_run_all(t_pipe_ctx *ctx, int *done)
{
	int	i;
	int	ok;

	i = 0;
	ok = 1;
	while (i < ctx->n && ok)
	{
		ok = pipe_step(ctx, i);
		if (ok)
			i++;
	}
	*done = i;
	return (ok);
}

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
	ctx.pids = (pid_t *)malloc(sizeof(pid_t) * ctx.n);
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
