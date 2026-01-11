/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 16:02:19 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:43:50 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

void	ms_child_exit(t_minishell *msh, t_ast *root, int code)
{
	if (root)
		free_ast(root);
	ms_clear(msh);
	exit(code);
}
/*
** 函数作用：执行一个 CMD 根节点（不在管道里）。
*/
int	exec_cmd_root(t_minishell *msh, t_ast *node)
{
	int	ret;

	ret = exec_cmd_node(msh, node, STDIN_FILENO, STDOUT_FILENO);
	return (ret);
}

/*
** 函数作用：执行一个 PIPE 根节点（整条管道）。
*/
int	exec_pipe_root(t_minishell *msh, t_ast *node)
{
	int	ret;

	ret = exec_pipe_node(msh, node, STDIN_FILENO, STDOUT_FILENO);
	return (ret);
}

/*
** 函数作用：执行 t_ast 总入口。
** 关键点：msh->last_exit_status 必须等于“最后执行的命令退出码”，这样 $? 和 exit 才对。
*/
int	exec_t_ast(t_minishell *msh, t_ast *root)
{
	int	status;

	status = 0;
	if (!msh || !root)
		return (0);
	setup_parent_exec_signals();
	if (root->type == NODE_PIPE)
		status = exec_pipe_root(msh, root);
	else if (root->type == NODE_CMD)
		status = exec_cmd_root(msh, root);
	msh->last_exit_status = status;
	setup_prompt_signals();
	return (status);
}
