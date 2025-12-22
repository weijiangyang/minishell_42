/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 16:02:19 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 16:02:26 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/exec.h"

/*
** 函数作用：
** 执行一个 CMD 根节点（不在管道里）。
** 这里只是把标准输入输出当作默认 fd 传下去。
*/
int	exec_cmd_root(t_minishell *msh, ast *node)
{
	int	ret;

	ret = 0;
	ret = exec_cmd_node(msh, node, STDIN_FILENO, STDOUT_FILENO);
	return (ret);
}

/*
** 函数作用：
** 执行一个 PIPE 根节点（整条管道）。
** 这里只是把标准输入输出当作默认 fd 传下去。
*/
int	exec_pipe_root(t_minishell *msh, ast *node)
{
	int	ret;

	ret = 0;
	ret = exec_pipe_node(msh, node, STDIN_FILENO, STDOUT_FILENO);
	return (ret);
}

/*
** 函数作用：
** 执行 AST 总入口。根据节点类型决定走 CMD 还是 PIPE。
** 并且把最终退出码保存到 msh->last_exit_status（也就是 $?）。
*/
int	exec_ast(t_minishell *msh, ast *root)
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
