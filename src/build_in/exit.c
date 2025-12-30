/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:57 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/30 03:48:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/build_in.h"
#include "../../include/minishell.h"

/*
** 函数作用：判断字符串是否为合法数字（允许 + 或 -）。
*/
static int	is_numeric(const char *s)
{
	int	i;

	if (!s || !s[0])
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i = i + 1;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit((unsigned char)s[i]))
			return (0);
		i = i + 1;
	}
	return (1);
}
/*
** 函数作用：把数字字符串转成 long（假设已通过 is_numeric）。
*/
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
		i = i + 1;
	}
	while (s[i])
	{
		val = val * 10 + (s[i] - '0');
		i = i + 1;
	}
	return (val * sign);
}

/*
** 函数作用：实现 exit 内置命令。
** 规则：
** - exit 无参数：退出码 = msh->last_exit_status
** - exit 参数不是数字：打印错误并 exit(2)
** - exit 参数过多：打印错误，返回 1（不退出 shell）
** - 数字参数：退出码取 unsigned char（0~255）
*/
int	builtin_exit(char **argv, t_minishell *msh)
{
	long	code;

	code = 0;
	if (argv && argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			ms_put3("minishell: exit: ", argv[1],
				": numeric argument required\n");
			exit(2);
		}
		if (argv[2])
		{
			ms_put3("minishell: exit: ", "too many arguments", "\n");
			return (1);
		}
		code = to_long(argv[1]);
		exit((unsigned char)code);
	}
	if (msh)
		exit(msh->last_exit_status);
	exit(0);
}
