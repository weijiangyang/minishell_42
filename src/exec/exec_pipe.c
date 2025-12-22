/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:27:46 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:19:24 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:27:46 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:00:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/exec.h"


/*
** 函数作用：
** 子进程执行一个子节点（CMD 或 PIPE），并用 exit 结束进程。
** 这样管道里每一段都会在自己的进程里跑。
*/
static void	child_run_node(t_minishell *msh, ast *sub, int in_fd, int out_fd)
{
	int	ret;

	ret = 0;
	if (dup_in_out_or_close(in_fd, out_fd) < 0)
		_exit(1);
	if (!sub)
		_exit(0);
	if (sub->type == NODE_CMD)
		child_exec_one(msh, sub, -1, -1);
	if (sub->type == NODE_PIPE)
	{
		ret = exec_pipe_node(msh, sub, -1, -1);
		_exit(ret);
	}
	_exit(0);
}

/*
** 函数作用：
** fork 出左边子进程：左边把输出写到管道写端。
*/
static pid_t	fork_left(t_minishell *msh, ast *node, int in_fd, int pfd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return (ms_perror("fork"), msh->last_exit_status = 1, -1);
	if (pid == 0)
	{
		close(pfd[0]);
		child_run_node(msh, node, in_fd, pfd[1]);
	}
	return (pid);
}

/*
** 函数作用：
** fork 出右边子进程：右边从管道读端读取输入。
*/
static pid_t	fork_right(t_minishell *msh, ast *node, int out_fd, int pfd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return (ms_perror("fork"), msh->last_exit_status = 1, -1);
	if (pid == 0)
	{
		close(pfd[1]);
		child_run_node(msh, node, pfd[0], out_fd);
	}
	return (pid);
}

/*
** 函数作用：
** 父进程关闭自己不再使用的 fd，防止 fd 泄漏。
*/
static void	close_parent_fds(int in_fd, int out_fd, int pfd[2])
{
	close(pfd[0]);
	close(pfd[1]);
	if (in_fd > STDERR_FILENO)
		close(in_fd);
	if (out_fd > STDERR_FILENO)
		close(out_fd);
}

/*
** 函数作用：
** 执行管道节点：左边写管道，右边读管道。
** 最终返回右边命令的退出码，和 bash 一样。
** 关键修复：
** - pipe() 失败时，关闭传进来的 in_fd/out_fd（非 STD），避免父进程泄露。
*/
int	exec_pipe_node(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	int		pfd[2];
	pid_t	left;
	pid_t	right;

	if (!node || !node->left || !node->right)
		return (0);
	if (pipe(pfd) < 0)
	{
		ms_perror("pipe");
		if (in_fd > STDERR_FILENO)
			close(in_fd);
		if (out_fd > STDERR_FILENO)
			close(out_fd);
		msh->last_exit_status = 1;
		return (1);
	}
	left = fork_left(msh, node->left, in_fd, pfd);
	if (left < 0)
		return (close_parent_fds(in_fd, out_fd, pfd), 1);
	right = fork_right(msh, node->right, out_fd, pfd);
	if (right < 0)
		return (close_parent_fds(in_fd, out_fd, pfd), waitpid(left, NULL, 0),
			1);
	close_parent_fds(in_fd, out_fd, pfd);
	return (wait_pair_set_right(msh, left, right));
}
