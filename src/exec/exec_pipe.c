/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:27:46 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/28 13:10:27 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "../../include/minishell.h"


/*
** 函数作用：等待所有子进程结束，并把“最后一个命令”的退出码写回 msh。
** 解释（初中生版）：
** 一条管道 a | b | c 会 fork 出 3 个子进程。
** bash 默认把“最后一个命令 c”的退出码当作整条管道的退出码。
*/
static void	wait_all_and_set_last(t_minishell *msh, pid_t *pids, int n)
{
	int	i;
	int	st;

	i = 0;
	st = 0;
	while (i < n)
	{
		if (waitpid(pids[i], &st, 0) == pids[n - 1])
			set_status_from_wait(msh, st);
		i = i + 1;
	}
}

/*
** 函数作用：执行 pipeline 的第 i 段（第 i 个命令）。
** 做的事（初中生版）：
** 1) 如果后面还有命令，就创建一个管道 pipe()
** 2) fork 一个子进程，让子进程跑 child_exec_one()
** 3) 父进程把不需要的 fd 关掉，并把“下一段要读的 fd”保存起来
** 返回值：成功 1，失败 0
*/
static int	pipe_run_step(t_minishell *msh, ast **arr, pid_t *pids,
		int *in_fd, int out_fd, int i, int n)
{
	int		pfd[2];
	int		child_out;
	pid_t	pid;

	pfd[0] = -1;
	pfd[1] = -1;
	if (i < n - 1)
	{
		if (pipe(pfd) < 0)
			return (0);
		child_out = pfd[1];
	}
	else
		child_out = out_fd;
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
		child_exec_one(msh, arr[i], *in_fd, child_out);
	pids[i] = pid;
	if (*in_fd > STDERR_FILENO)
		close(*in_fd);
	if (pfd[1] != -1)
		close(pfd[1]);
	*in_fd = pfd[0];
	return (1);
}

/*
** 函数作用：执行“任意长度”的 pipeline：a|b|c|...
** 参数：
** - node：PIPE 根节点
** - in_fd/out_fd：本条 pipeline 的输入输出（一般就是 STDIN/STDOUT）
** 返回值：成功 1，失败 0
**
** 重点行为（贴 bash）：
** - 即使前面命令不存在（exit 127），后面的命令也照样会 fork 执行
** - 整条管道的退出码 = 最后一个命令的退出码（bash 默认 pipefail 关闭）
*/
int	exec_pipe_node(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	ast		**arr;
	pid_t	*pids;
	int		n;
	int		i;
	int		ok;

	arr = NULL;
	n = 0;
	ok = pipe_collect(node, &arr, &n);
	if (!ok)
		return (0);
	pids = (pid_t *)malloc(sizeof(*pids) * n);
	if (!pids)
		return (free(arr), 0);
	i = 0;
	while (i < n && ok)
	{
		ok = pipe_run_step(msh, arr, pids, &in_fd, out_fd, i, n);
		i = i + 1;
	}
	if (in_fd > STDERR_FILENO)
		close(in_fd);
	wait_all_and_set_last(msh, pids, i);
	free(pids);
	free(arr);
	return (ok);
}
