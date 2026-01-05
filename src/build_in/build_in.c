/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:23:18 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:23:22 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../../include/build_in.h"
#include "../../include/minishell.h"
#include "../../libft/libft.h"

// 检测命令是否为内置命令，返回 1 如果是，否则 0
int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(cmd, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "env", 4) == 0)
		return (1);
	return (0);
}

// 执行内置命令，返回退出码
int	exec_builtin(ast *node, t_env **env, t_minishell *msh)
{
	if (!node || !node->argv || !node->argv[0])
		return (1);
	if (ft_strncmp(node->argv[0], "cd", 3) == 0)
		return (ft_cd(node->argv, env));
	if (ft_strncmp(node->argv[0], "echo", 5) == 0)
		return (ft_echo(node->argv));
	if (ft_strncmp(node->argv[0], "pwd", 4) == 0)
		return (builtin_pwd());
	if (ft_strncmp(node->argv[0], "export", 7) == 0)
		return (builtin_export(node->argv, env));
	if (ft_strncmp(node->argv[0], "env", 4) == 0)
		return (builtin_env(node->argv, *env));
	if (ft_strncmp(node->argv[0], "unset", 6) == 0)
		return (builtin_unset(node->argv, env));
	if (ft_strncmp(node->argv[0], "exit", 5) == 0)
		return (builtin_exit(node->argv, msh));
	return (1);
}
