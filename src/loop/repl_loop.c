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

/*
** 作用：程序的核心发动机，一直循环读取并运行命令，直到用户想离开。
*/
void	repl_loop(t_minishell *ms)
{
	char	*acc;
	int		stop;

	acc = NULL;
	stop = 0;
	while (!stop && ms && ms->should_exit == 0)
		stop = repl_step(ms, &acc);
	repl_free_acc(&acc);
}
