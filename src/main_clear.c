/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_clear.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:54:14 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 18:54:49 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

void	release_matrix(char **m)
{
	int	i;

	if (!m)
		return ;
	i = 0;
	while (m[i])
	{
		free(m[i]);
		i++;
	}
	free(m);
}

void	ms_clear(t_minishell *ms)
{
	if (!ms)
		return ;
	if (ms->lexer)
		clear_list(&ms->lexer);
	if (ms->paths)
	{
		release_matrix(ms->paths);
		ms->paths = NULL;
	}
	if (ms->envp)
	{
		release_matrix(ms->envp);
		ms->envp = NULL;
	}
	if (ms->env)
	{
		free_env(ms->env);
		ms->env = NULL;
	}
}
