/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 00:15:10 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/30 06:42:26 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

/*
** 函数作用：判断 path 是否是目录。
** 目录不能当可执行文件运行：应报错 "Is a directory"，并返回 126。
*/
static int	path_is_dir(const char *path)
{
	struct stat	st;

	if (!path)
		return (0);
	if (stat(path, &st) != 0)
		return (0);
	if (S_ISDIR(st.st_mode))
		return (1);
	return (0);
}

/*
** 函数作用：当 execve 因为 EACCES 失败时，尝试模拟 bash 的
** “bad interpreter: Permission denied” 报错。
** 只在脚本文件以 "#!" 开头时才打印。
** 返回：1 表示已经打印了；0 表示不适用。
*/
static int	try_print_bad_interpreter(const char *cmd, const char *path,
		int err)
{
	int		fd;
	ssize_t	n;
	char	buf[200];
	int		i;
	int		j;

	if (!cmd || !path)
		return (0);
	if (err != EACCES)
		return (0);
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	n = read(fd, buf, 199);
	close(fd);
	if (n < 2)
		return (0);
	buf[n] = '\0';
	if (buf[0] != '#' || buf[1] != '!')
		return (0);
	i = 2;
	while (buf[i] == ' ' || buf[i] == '\t')
		i++;
	j = i;
	while (buf[j] && buf[j] != ' ' && buf[j] != '\t' && buf[j] != '\n')
		j++;
	if (j == i)
		return (0);
	buf[j] = '\0';
	ms_err_bad_interpreter(cmd, &buf[i]);
	return (1);
}

/*
** 函数作用：检查 heredoc 是否失败。
** 你的项目结构里 heredoc_fd < 0 就表示 heredoc 没准备好/失败。
*/
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
** 函数作用：在子进程里关闭 heredoc 的 fd，避免 fd 泄露。
*/
static void	close_heredoc_fds(t_redir *r)
{
	while (r)
	{
		if (r->type == HEREDOC && r->heredoc_fd >= 0)
			close(r->heredoc_fd);
		r = r->next;
	}
}

/*
** 函数作用：子进程执行外部命令（PATH 搜索 + execve）。
** 返回码规则（贴 bash）：
** - command not found -> 127
** - permission denied / is a directory -> 126
*/
static void	child_exec_external(t_minishell *msh, ast *node)
{
	char	**argv;
	char	*path;
	int		err;
	int		code;

	argv = node->argv;
	if (!argv || !argv[0])
		exit(0);
	change_envp(msh->env, &msh->envp);
	path = find_cmd_path(msh, argv[0]);
	if (!path)
	{
		ms_err_cmd_not_found(argv[0]);
		exit(127);
	}
	if (path_is_dir(path))
	{
		ms_err_exec(argv[0], EISDIR);
		free(path);
		exit(126);
	}
	execve(path, argv, msh->envp);
	err = errno;
	if (try_print_bad_interpreter(argv[0], path, err))
	{
		free(path);
		exit(126);
	}
	ms_err_exec(argv[0], err);
	code = 127;
	if (err == EACCES || err == EISDIR)
		code = 126;
	free(path);
	exit(code);
}

// ** 函数作用：子进程执行一个命令节点（包含 builtin / external）。
void	child_exec_one(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	int	new_in;
	int	new_out;

	if (!msh || !node || node->type != NODE_CMD)
		exit(1);
	if (has_bad_heredoc(node->redir))
		exit(1);
	new_in = in_fd;
	new_out = out_fd;
	if (new_in > STDERR_FILENO && node->argv && node->argv[0]
		&& is_builtin(node->argv[0]))
	{
		close(new_in);
		new_in = -1;
	}
	if (apply_redir_list(node->redir, &new_in, &new_out) < 0)
		exit(1);
	close_heredoc_fds(node->redir);
	if (dup_in_out_or_close(new_in, new_out) < 0)
		exit(1);
	if (node->argv && node->argv[0] && is_builtin(node->argv[0]))
		exit(exec_builtin(node, &msh->env, msh));
	child_exec_external(msh, node);
	exit(1);
}
