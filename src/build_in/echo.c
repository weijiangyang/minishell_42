/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:34 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:16:37 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

static int	parse_echo_flags(char **argv, int *start_index)
{
	int	i;
	int	j;
	int	print_newline;

	i = 1;
	print_newline = 1;
	while (argv[i] && argv[i][0] == '-')
	{
		j = 1;
		if (argv[i][j] == '\0')
			break ;
		while (argv[i][j] == 'n')
			j++;
		if (argv[i][j] != '\0')
			break ;
		print_newline = 0;
		i++;
	}
	*start_index = i;
	return (print_newline);
}

static void	echo_print_args(char **argv, int start_index)
{
	int	i;

	i = start_index;
	while (argv[i])
	{
		ft_putstr_fd(argv[i], STDOUT_FILENO);
		if (argv[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
}

int	ft_echo(char **argv)
{
	int	print_newline;
	int	start_index;

	print_newline = parse_echo_flags(argv, &start_index);
	echo_print_args(argv, start_index);
	if (print_newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
