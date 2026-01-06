/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:48 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 15:02:21 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

/*
** 函数作用：
**   minishell 的主循环：不停读取用户输入并执行。
**   acc 用来保存“半截输入”，支持续行（比如：引号没闭合）。
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
