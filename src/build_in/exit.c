/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:24:07 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/13 15:29:14 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h"
#include <unistd.h>

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

static void	set_exit_state(t_minishell *msh, int ret)
{
	if (msh)
	{
		msh->last_exit_status = ret;
		msh->exit_code = ret;
		msh->should_exit = 1;
	}
}

// int	builtin_exit(char **argv, t_minishell *msh)
// {
// 	int	ret;

// 	ft_putendl_fd("exit", 2);
// 	ret = 0;
// 	if (argv && argv[1])
// 	{
// 		if (!is_numeric(argv[1]))
// 		{
// 			ms_put3("minishell: exit: ", argv[1],
// 				": numeric argument required\n");
// 			ret = 2;
// 		}
// 		else if (argv[2])
// 		{
// 			ms_put3("minishell: exit: ", "too many arguments", "\n");
// 			if (msh)
// 				msh->last_exit_status = 1;
// 			return (1);
// 		}
// 		else
// 			ret = (unsigned char)to_long(argv[1]);
// 	}
// 	else if (msh)
// 		ret = msh->last_exit_status;
// 	return (set_exit_state(msh, ret), ret);
// }
/* src/build_in/exit.c */

int	builtin_exit(char **argv, t_minishell *msh)
{
	int	ret;

	if (isatty(0))
		ft_putendl_fd("exit", 2);
	ret = 0;
	if (argv && argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			ms_put3("minishell: exit: ", argv[1],
				": numeric argument required\n");
			return (set_exit_state(msh, 2), 2);
		}
		if (argv[2])
		{
			ms_put3("minishell: exit: ", "too many arguments", "\n");
			if (msh)
				msh->last_exit_status = 1;
			return (1);
		}
		ret = (unsigned char)to_long(argv[1]);
	}
	else if (msh)
		ret = msh->last_exit_status;
	return (set_exit_state(msh, ret), ret);
}
