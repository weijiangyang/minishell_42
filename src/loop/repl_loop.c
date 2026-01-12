/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:48 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 19:27:10 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

void	repl_loop(t_minishell *ms)
{
	char	*acc;
	int		stop;

	acc = NULL;
	stop = 0;
	if (!ms)
		return ;
	while (!stop && ms->should_exit == 0)
	{
		stop = repl_step(ms, &acc);
	}
	repl_free_acc(&acc);
}
