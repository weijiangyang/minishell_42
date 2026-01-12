/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 17:30:03 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 15:13:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

void	ms_put3(const char *a, const char *b, const char *c)
{
	if (a)
		ft_putstr_fd((char *)a, STDERR_FILENO);
	if (b)
		ft_putstr_fd((char *)b, STDERR_FILENO);
	if (c)
		ft_putstr_fd((char *)c, STDERR_FILENO);
}

void	ms_err_cmd_not_found(const char *cmd)
{
	ms_put3("minishell: ", cmd, ": command not found\n");
}

void	ms_err_exec(const char *cmd, int err)
{
	ms_put3("minishell: ", cmd, ": ");
	ft_putstr_fd(strerror(err), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

void	ms_err_redir(const char *file, int err)
{
	ms_put3("minishell: ", file, ": ");
	ft_putstr_fd(strerror(err), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

void	ms_err_bad_interpreter(const char *cmd, const char *interp)
{
	ms_put3("minishell: ", cmd, ": ");
	ms_put3(interp, ": bad interpreter: Permission denied\n", NULL);
}
