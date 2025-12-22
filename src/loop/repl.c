/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:48 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/22 15:17:13 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "repl.h"
#include "../../include/minishell.h"

/*
** 函数作用：
**   minishell 的主循环：不停读取用户输入并执行。
** 参数：
**   ms：全局上下文（环境变量、状态码、lexer 链表等都在里面）
*/
void	repl_loop(t_minishell *ms)
{
	char	*acc;
	int		stop;

	acc = NULL;
	stop = 0;
	while (stop == 0)
		stop = repl_step(ms, &acc);
	if (acc)
		free(acc);
}
