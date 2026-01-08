/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:24:07 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/06 15:01:46 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h" // 你的 t_minishell 定义

// 判断字符串是否为合法数字
static int	is_numeric(const char *s)
{
	int	i;

	i = 0;
	if (!s || !s[0])
		return (0);
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

// 把数字字符串转成 long（假设已通过 is_numeric）
static long	to_long(const char *s)
{
	int		i;
	int		sign;
	long	val;

	i = 0;
	sign = 1;
	val = 0;
	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	while (s[i])
	{
		val = val * 10 + (s[i] - '0');
		i++;
	}
	return (val * sign);
}

/**
 * @brief exit 内置命令的完整实现。
 * * 行为逻辑：
 * 1. 终端提示：如果是在交互式终端且没有管道，打印 "exit"。
 * 2. 参数校验：
 * - 非数字参数：如果第一个参数不是数字，报错并以状态码 2 退出。
 * - 参数过多：如果有超过一个参数，报错并不退出（保持 Shell 运行）。
 * 3. 状态码转换：将字符串参数转换为 long，并截断为 unsigned char (0-255)。
 * 4. 默认行为：若无参数，则以最近一次命令的退出状态 (last_exit_status) 退出。
 * * @param argv 命令参数数组。
 * @param msh  全局上下文，用于获取最后的退出状态。
 * @return int 仅在参数过多不退出时返回 1。
 */
#include "../../include/build_in.h"
#include "../../include/minishell.h"
#include <unistd.h> // isatty, STDIN_FILENO, STDOUT_FILENO

int	builtin_exit(char **argv, t_minishell *msh)
{
	long	code;
	int		ret;

	code = 0;
	ret = 0;
	/* 规则：exit 参数不是数字 -> 打错并“请求退出”，退出码 2 */
	if (argv && argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			ms_put3("minishell: exit: ", argv[1],
				": numeric argument required\n");
			ret = 2;
			if (msh)
			{
				msh->last_exit_status = ret;
				msh->exit_code = ret;
				msh->should_exit = 1;
			}
			return (ret);
		}
		/* 规则：exit 参数过多 -> 不退出，返回 1 */
		if (argv[2])
		{
			ms_put3("minishell: exit: ", "too many arguments", "\n");
			if (msh)
				msh->last_exit_status = 1;
			return (1);
		}
		code = to_long(argv[1]);
		ret = (unsigned char)code;
	}
	else
	{
		ret = (msh != NULL) ? msh->last_exit_status : 0;
	}
	/* 只有真的要退出时，交给外层 loop 收尾 */
	if (msh)
	{
		msh->last_exit_status = ret;
		msh->exit_code = ret;
		msh->should_exit = 1;
	}
	/* bash 交互模式会打印 exit(Ctrl-D 那里也是这么做的） */
	if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "exit\n", 5);
	return (ret);
}
