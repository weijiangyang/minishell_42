/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 00:15:10 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:20:31 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 00:15:10 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:00:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/exec.h"
#include "error.h"
#include <errno.h>

static int	has_bad_heredoc(t_redir *r)
{
	while (r)
	{
		if (r->type == HEREDOC && r->heredoc_fd < 0)
			return (1);
		r = r->next;
	}
	return (0);
}

/*
** 函数作用：
** 子进程完成 dup2 之后，关闭该命令 redir 链表里所有 heredoc_fd：
** - 避免外部命令 execve 后继承多余 fd（fd 泄露检查会抓这个）
** - 只在子进程关；并把 heredoc_fd 置为 -1，避免子进程路径上 double close
*/
static void	close_heredoc_fds(t_redir *r)
{
	while (r)
	{
		if (r->type == HEREDOC && r->heredoc_fd > STDERR_FILENO)
		{
			close(r->heredoc_fd);
			r->heredoc_fd = -1;
		}
		r = r->next;
	}
}

/*
** 函数作用：
** 子进程执行外部命令：
** 1) 把 env 链表转成 envp 数组
** 2) 找到可执行路径
** 3) execve 失败打印 bash 风格错误，并按 errno 映射 126/127
*/
static void	child_exec_external(t_minishell *msh, char **argv)
{
	char	*path;
	int		err;
	int		exit_code;

	path = NULL;
	change_envp(msh->env, &msh->envp);
	path = find_cmd_path(msh, argv[0]);
	if (!path)
	{
		ms_err_cmd_not_found(argv[0]);
		_exit(127);
	}
	execve(path, argv, msh->envp);
	err = errno;
	exit_code = 127;
	if (err == EACCES || err == EISDIR)
		exit_code = 126;
	ms_err_exec(argv[0], err);
	free(path);
	_exit(exit_code);
}

/*
** 函数作用：
** 子进程执行一个命令节点：
** 先接好管道 fd，再应用重定向，再决定跑 builtin 还是外部命令。
** 关键修复：
** - 即使命令为空（只有重定向），也要先处理重定向副作用/报错，再 _exit(0)。
** - dup2 完成后关闭 heredoc_fd，避免外部命令继承多余 fd。
*/
void	child_exec_one(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	int	new_in;
	int	new_out;

	new_in = STDIN_FILENO;
	new_out = STDOUT_FILENO;
	if (!node)
		_exit(0);
	if (has_bad_heredoc(node->redir))
		_exit(130);
	if (dup_in_out_or_close(in_fd, out_fd) < 0)
		_exit(1);
	if (apply_redir_list(node->redir, &new_in, &new_out) < 0)
		_exit(1);
	if (dup_in_out_or_close(new_in, new_out) < 0)
		_exit(1);
	close_heredoc_fds(node->redir);
	if (!node->argv || !node->argv[0])
		_exit(0);
	if (is_builtin(node->argv[0]))
		_exit(exec_builtin(node, &msh->env));
	child_exec_external(msh, node->argv);
}
