/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 15:15:42 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 15:15:44 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include "error.h"

/*
** 函数作用：只关闭“不是标准输入/输出/错误”的 fd，避免误关 stdin/stdout。
*/
static void	close_keep_std(int fd)
{
	if (fd > STDERR_FILENO)
		close(fd);
}

/**
 * @brief 在父进程中执行内置命令，并管理 FD 重定向。
 * * 流程如下：
 * 1. 备份标准 IO：保存当前的 STDIN/STDOUT，以便后续恢复。
 * 2. 处理重定向：应用命令中指定的重定向（如 > filename）。
 * 3. 替换 FD：使用 dup2 将标准 IO 指向重定向的目标。
 * 4. 执行命令：运行必须在父进程执行的内置命令（cd, export 等）。
 * 5. 环境更新：同步环境变量链表与 envp 数组，更新 PATH。
 * 6. 状态恢复：将标准 IO 恢复到备份时的状态。
 * * @param msh    全局上下文。
 * @param node   当前 AST 节点（包含命令名和参数）。
 * @param in_fd  初始输入 FD（通常来自管道）。
 * @param out_fd 初始输出 FD（通常来自管道）。
 * @return int   返回内置命令的退出状态码。
 */
int	run_builtin_parent_logic(t_minishell *msh, ast *node, int in_fd,
		int out_fd)
{
	t_fd_save	save;
	int			new_in;
	int			new_out;
	int			ret;

	ret = 1;
	if (save_std_fds(&save) != 0)
		return (close_keep_std(in_fd), close_keep_std(out_fd), 1);
	new_in = in_fd;
	new_out = out_fd;
	if (apply_redir_list(node->redir, &new_in, &new_out) < 0)
		return (restore_std_fds(&save), 1);
	if (dup_in_out_or_close(new_in, new_out) < 0)
		return (restore_std_fds(&save), 1);
	ret = exec_builtin(node, &msh->env, msh);
	change_envp(msh->env, &msh->envp);
	exec_refresh_paths(msh);
	restore_std_fds(&save);
	return (ret);
}

