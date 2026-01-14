/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+                 */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 15:15:42 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 15:15:44 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

static void	close_keep_std(int fd)
{
	if (fd > STDERR_FILENO)
		close(fd);
}

int	run_builtin_parent_logic(t_minishell *msh, t_ast *node, int in_fd,
		int out_fd)
{
	t_fd_save	save;
	int			new_in;
	int			new_out;
	int			ret;

	if (save_std_fds(&save) != 0)
	{
		close_keep_std(in_fd);
		close_keep_std(out_fd);
		return (1);
	}
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
