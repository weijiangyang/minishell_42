/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:25:55 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:19:02 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:25:55 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:00:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/exec.h"


static void	close_keep_std(int *fd)
{
	if (!fd)
		return ;
	if (*fd > STDERR_FILENO)
	{
		close(*fd);
		*fd = -1;
	}
}

/*
** 函数作用：
** 针对一种重定向，打开一个新的 fd：
** < 读文件；> 覆盖写；>> 追加写；<< 用 heredoc_fd（dup 一份给执行阶段用）。
*/
static int	open_redir_fd(t_redir *r)
{
	int	fd;

	fd = -1;
	if (r->type == REDIR_INPUT)
		fd = open(r->filename, O_RDONLY);
	else if (r->type == REDIR_OUTPUT)
		fd = open(r->filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else if (r->type == REDIR_APPEND)
		fd = open(r->filename, O_CREAT | O_WRONLY | O_APPEND, 0644);
	else if (r->type == HEREDOC && r->heredoc_fd >= 0)
		fd = dup(r->heredoc_fd);
	return (fd);
}

/*
** 函数作用：
** 输入类重定向放到 in_fd，输出类重定向放到 out_fd。
** 后面的重定向会覆盖前面的，所以要先关掉旧的 fd。
*/
static void	set_redir_fd(t_redir *r, int fd, int *in_fd, int *out_fd)
{
	if (r->type == REDIR_INPUT || r->type == HEREDOC)
	{
		close_keep_std(in_fd);
		*in_fd = fd;
	}
	else
	{
		close_keep_std(out_fd);
		*out_fd = fd;
	}
}

/*
** 函数作用：
** 按链表顺序应用所有重定向，得到最终要接到 stdin/stdout 的 fd。
** 关键修复：
** - 如果中途某一步打开失败：打印错误，并关闭之前已打开的 *in_fd *out_fd
**  （非标准 fd），避免父进程 fd 泄露。
*/
int	apply_redir_list(t_redir *r, int *in_fd, int *out_fd)
{
	int	fd;
	int	err;

	while (r)
	{
		fd = open_redir_fd(r);
		if (fd < 0)
		{
			err = errno;
			if (r->filename)
				ms_err_redir(r->filename, err);
			else
				ms_err_redir("redir", err);
			close_keep_std(in_fd);
			close_keep_std(out_fd);
			return (-1);
		}
		set_redir_fd(r, fd, in_fd, out_fd);
		r = r->next;
	}
	return (0);
}
