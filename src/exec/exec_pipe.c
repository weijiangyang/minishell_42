/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:27:46 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/27 16:51:19 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "../../include/minishell.h"

/*
** 函数作用：把 PIPE 树摊平成数组 [a,b,c]（在新文件里实现）
** 参数含义：root(PIPE 根), out_arr(输出数组), out_n(输出段数)
** 返回值：成功 1，失败 0
*/

typedef struct s_pipe_run
{
	ast		**arr;
	pid_t	*pids;
	int		n;
	int		i;
	int		in;
	int		out_fd;
	int		pfd[2];
}			t_pipe_run;

/*
** 函数作用：等所有子进程结束，并把“最后一个命令”的退出码写回 msh
** 参数含义：msh(全局上下文), pids(pid 数组), n(进程数)
*/
static void	pipe_wait_last(t_minishell *msh, pid_t *pids, int n)
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
** 函数作用：执行 pipeline 的“第 i 段”，并在父进程更新下一段的输入 fd
** 参数含义：msh(全局上下文), st(管道运行状态，包含第几段/哪些fd)
** 返回值：成功 1，失败 0
*/
static int	pipe_step(t_minishell *msh, t_pipe_run *st)
{
	pid_t	pid;
	int		out;

	st->pfd[0] = -1;
	st->pfd[1] = -1;
	if (st->i < st->n - 1 && pipe(st->pfd) < 0)
		return (0);
	out = st->out_fd;
	if (st->i < st->n - 1)
		out = st->pfd[1];
	pid = fork();
	if (pid < 0)
		return (close(st->pfd[0]), close(st->pfd[1]), 0);
	if (pid == 0)
		child_exec_one(msh, st->arr[st->i], st->in, out);
	st->pids[st->i] = pid;
	if (st->in != -1)
		close(st->in);
	if (st->pfd[1] != -1)
		close(st->pfd[1]);
	st->in = st->pfd[0];
	st->i = st->i + 1;
	return (1);
}

/*
** 函数作用：执行“任意长度”的 pipeline：a|b|c|...
** 参数含义：msh(全局上下文), node(PIPE 根), in_fd(输入fd), out_fd(输出fd)
** 返回值：成功 1，失败 0
*/
int	exec_pipe_node(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	t_pipe_run	st;
	int			ok;

	st.arr = NULL;
	st.n = 0;
	ok = pipe_collect(node, &st.arr, &st.n);
	if (ok == 0)
		return (0);
	st.pids = (pid_t *)malloc(sizeof(*st.pids) * st.n);
	if (st.pids == NULL)
		return (free(st.arr), 0);
	st.i = 0;
	st.in = in_fd;
	st.out_fd = out_fd;
	while (st.i < st.n && ok == 1)
		ok = pipe_step(msh, &st);
	if (st.in != -1)
		close(st.in);
	pipe_wait_last(msh, st.pids, st.i);
	free(st.pids);
	free(st.arr);
	return (ok);
}
