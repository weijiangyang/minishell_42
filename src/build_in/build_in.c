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



#include "build_in.h"
#include "minishell.h"

/**
 * @brief 检查给定字符串是否匹配 Minishell 支持的内置命令。
 * * 内置命令与外部命令的区别：
 * - 内置命令：在 Shell 进程内部执行，可以直接改变 Shell 的状态（如 cd, export）。
 * - 外部命令：需要 fork 子进程并执行 execve。
 * * 支持的命令列表：
 * 1. cd     - 改变当前工作目录
 * 2. echo   - 打印文本
 * 3. pwd    - 显示当前绝对路径
 * 4. exit   - 退出 Shell
 * 5. export - 设置或显示环境变量
 * 6. unset  - 删除环境变量
 * 7. env    - 显示当前环境变量
 * * @param cmd 待检查的命令字符串。
 * @return int 如果是内置命令返回 1；否则返回 0。
 */
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

/**
 * @brief 执行匹配的内置命令并返回其退出状态码。
 * * 该函数充当内置命令的“开关柜”：
 * 1. 基础检查：确保节点、参数数组及命令名均有效。
 * 2. 逻辑分发：通过 ft_strcmp 匹配命令字符串。
 * 3. 状态返回：调用对应的具体实现函数，并将它们的返回值（0 表示成功）向上传递。
 * * @param node 包含命令及其参数 (argv) 的 AST 节点。
 * @param env  指向环境变量链表的指针地址（某些命令如 cd/export 需要修改它）。
 * @param msh  全局上下文，主要用于 exit 命令清理资源。
 * @return int 返回内置命令执行后的退出状态码（0-255）。
 */
int	exec_builtin(ast *node, t_env **env, t_minishell *msh)
{
	if (!node || !node->argv || !node->argv[0])
		return (1);
	if (ft_strcmp(node->argv[0], "cd") == 0)
		return (ft_cd(node->argv, env));
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
