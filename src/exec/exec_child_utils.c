/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 22:57:11 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:01:06 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "exec.h"
#include "minishell.h"

/*
** 这个函数就像个安检员，专门检查给定的路径是不是一个文件夹。
** 为什么要检查？因为我们不能把文件夹当成程序来运行，如果用户非要运行，
** 后面要给用户报错说 "Is a directory"。
** @param path: 要检查的路径字符串。
** @return: 如果是文件夹返回 1，否则返回 0。
*/
int	path_is_dir(const char *path)
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
** 这个函数用来检查这一串重定向操作里，有没有哪个 Heredoc（<<）出问题了。
** 如果 heredoc_fd 小于 0，说明之前准备输入的时候就失败了（比如被用户取消了）。
** @param r: 重定向列表的头节点。
** @return: 有坏掉的 heredoc 返回 1，一切正常返回 0。
*/
int	has_bad_heredoc(t_redir *r)
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
** 这个函数只负责关闭一个命令节点下所有的 Heredoc 打开的文件。
** 这是为了防止子进程运行结束后，这些临时文件还开着，造成资源泄露。
** @param r: 重定向列表的头节点。
*/
static void	close_heredoc_fds(t_redir *r)
{
	while (r)
	{
		if (r->type == HEREDOC && r->heredoc_fd >= 0)
		{
			close(r->heredoc_fd);
			r->heredoc_fd = -1;
		}
		r = r->next;
	}
}

/*
** 这个函数是个递归清理工，它会把整个命令树（左边、右边、子命令）里
** 所有的 Heredoc 打开的文件都关掉。
** 为什么要这么彻底？因为管道（Pipeline）里的其他命令也会继承这些文件，
** 不关掉的话，别的程序也能看到这些不该看到的文件。
** @param node: 抽象语法树（AST）的当前节点。
*/
void	close_all_heredoc_fds(t_ast *node)
{
	if (!node)
		return ;
	if (node->type == NODE_CMD)
		close_heredoc_fds(node->redir);
	if (node->sub)
		close_all_heredoc_fds(node->sub);
	if (node->left)
		close_all_heredoc_fds(node->left);
	if (node->right)
		close_all_heredoc_fds(node->right);
}
