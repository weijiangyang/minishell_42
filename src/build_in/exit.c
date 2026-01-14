/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:24:07 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h" // 你的 t_minishell 定义

/*
** 检查字符串是否为有效的 long 类型并进行转换。
** 处理正负号，并针对 LONG_MAX 和 LONG_MIN 进行溢出/下溢检测。
** 成功返回 1，如果不是数字或发生溢出则返回 0。
*/
static int	ft_is_long(const char *s, long *res)
{
	int				i;
	int				sign;
	unsigned long	val;

	i = 0;
	sign = 1;
	val = 0;
	if (s[i] == '+' || s[i] == '-')
		if (s[i++] == '-')
			sign = -1;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		val = val * 10 + (s[i++] - '0');
		// 针对正数溢出的检测
		if (sign == 1 && val > (unsigned long)LONG_MAX)
			return (0);
		// 针对负数下溢的检测 (处理 LONG_MIN 的绝对值比 LONG_MAX 大 1 的特殊情况)
		if (sign == -1 && val > (unsigned long)LONG_MAX + 1)
			return (0);
	}
	*res = (long)(val * sign);
	return (1);
}

/*
** 处理 'exit' 内建命令。
** 1. 检查非数字参数：如果是，报错并退出 Shell，状态码设为 2。
** 2. 检查参数过多：如果是，报错但不退出 Shell，状态码设为 1。
** 3. 更新最后退出状态码并返回最终的退出码 (转换后的 unsigned char, 即 0-255)。
*/
int	builtin_exit(char **argv, t_minishell *msh)
{
	long	ret;

	// 如果是在交互式终端，打印 "exit"
	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", 2);
	if (argv && argv[1])
	{
		// 校验第一个参数是否为有效 long 数字
		if (!ft_is_long(argv[1], &ret))
		{
			ms_put3("minishell: exit: ", argv[1],
				": numeric argument required\n");
			msh->last_exit_status = 2;
			msh->should_exit = 1;
			return (2);
		}
		// 如果数字参数后面还有多余参数，报错但不退出
		if (argv[2])
		{
			ms_put3("minishell: exit: ", "too many arguments", "\n");
			msh->last_exit_status = 1;
			return (1);
		}
	}
	else
		// 如果没有参数，使用上一个命令的退出状态
		ret = msh->last_exit_status;
	// 正常退出：强制转换为 unsigned char (0-255)
	msh->last_exit_status = (unsigned char)ret;
	msh->should_exit = 1;
	return ((unsigned char)ret);
}