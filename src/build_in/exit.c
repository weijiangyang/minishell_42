/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:57 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/30 03:20:03 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/build_in.h"
#include "../../include/minishell.h"

/*
** 函数作用：判断字符串是不是合法数字（可带 + 或 -）。
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

int	builtin_exit(char **argv, t_minishell *msh)
{
	long	status;

	// 增加long_min, long_max 整数溢出的检测
	status = 0;
	if (argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			ms_put3("minishell: exit: ", argv[1],
				": numeric argument required\n");
			exit(2);
		}
		status = atol(argv[1]); // 支持大数字
		if (argv[2])
		{
			ms_put3("minishell: exit: ", "too many arguments", "\n");

			// Bash: 多参数时报错，但不退出 shell
			return (1);
		}
		// exit code 只保留 0~255
		exit((unsigned char)status);
	}
	exit(msh->last_exit_status);
}
