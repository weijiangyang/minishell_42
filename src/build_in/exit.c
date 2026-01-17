/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:24:07 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/15 20:01:20 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h"

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
		if (sign == 1 && val > (unsigned long)LONG_MAX)
			return (0);
		if (sign == -1 && val > (unsigned long)LONG_MAX + 1)
			return (0);
	}
	*res = (long)(val * sign);
	return (1);
}

int	builtin_exit(char **argv, t_minishell *msh)
{
	long	ret;

	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", 2);
	if (argv && argv[1])
	{
		if (!ft_is_long(argv[1], &ret))
		{
			ms_put3("minishell: exit: ", argv[1],
				": numeric argument required\n");
			msh->last_exit_status = 2;
			return (msh->should_exit = 1, msh->last_exit_status);
		}
		if (argv[2])
		{
			ms_put3("minishell: exit: ", "too many arguments", "\n");
			if (msh)
				msh->last_exit_status = 1;
			return (1);
		}
	}
	else
		ret = msh->last_exit_status;
	msh->last_exit_status = (unsigned char)ret;
	return (msh->should_exit = 1, (unsigned char)ret);
}
