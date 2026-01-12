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

int	exec_cmd_root(t_minishell *msh, t_ast *node)
{
	int	ret;

	ret = exec_cmd_node(msh, node, STDIN_FILENO, STDOUT_FILENO);
	return (ret);
}

int	exec_pipe_root(t_minishell *msh, t_ast *node)
{
	int	ret;

	ret = exec_pipe_node(msh, node, STDIN_FILENO, STDOUT_FILENO);
	return (ret);
}

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
