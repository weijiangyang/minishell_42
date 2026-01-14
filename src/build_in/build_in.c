/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:23:18 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:23:22 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h"

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	return (0);
}

int	exec_builtin(t_ast *node, t_env **env, t_minishell *msh)
{
	if (!node || !node->argv || !node->argv[0])
		return (1);
	if (ft_strcmp(node->argv[0], "cd") == 0)
		return (ft_cd(node->argv, env, msh));
	if (ft_strcmp(node->argv[0], "echo") == 0)
		return (ft_echo(node->argv));
	if (ft_strcmp(node->argv[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(node->argv[0], "export") == 0)
		return (builtin_export(node->argv, env));
	if (ft_strcmp(node->argv[0], "env") == 0)
		return (builtin_env(node->argv, *env));
	if (ft_strcmp(node->argv[0], "unset") == 0)
		return (builtin_unset(node->argv, env));
	if (ft_strcmp(node->argv[0], "exit") == 0)
		return (builtin_exit(node->argv, msh));
	return (1);
}
