/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_step.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:45 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/22 15:20:47 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "repl.h"

/*
** 函数作用：
**   主提示符下 Ctrl-D：打印 exit 并让外层 loop 退出。
** 返回：
**   1 表示退出
*/
static int	step_eof_main(void)
{
	printf("exit\n");
	return (1);
}

/*
** 函数作用：
**   续行阶段 Ctrl-D：打印 bash 风格错误，丢弃 acc，回到主提示符。
** 参数：
**   ms：全局上下文（用来拿未闭合引号类型）
**   acc：累计输入（会被 free 并置 NULL）
** 返回：
**   0 表示继续循环
*/
static int	step_eof_more(t_minishell *ms, char **acc)
{
	char	q;

	q = '"';
	if (ms && ms->lexer_unclosed_quote)
		q = ms->lexer_unclosed_quote;
	fprintf(stderr, "bash: unexpected EOF while looking for matching `%c'\n",
		q);
	fprintf(stderr, "bash: syntax error: unexpected end of file\n");
	if (ms)
		ms->last_exit_status = 258;
	if (acc && *acc)
		free(*acc);
	if (acc)
		*acc = NULL;
	if (ms)
		ms->raw_line = NULL;
	return (0);
}

/*
** 函数作用：
**   统一处理 readline 返回 NULL 的情况（Ctrl-D / EOF）。
** 参数：
**   ms：全局上下文
**   acc：累计输入
** 返回：
**   1 退出；0 继续
*/
static int	step_handle_eof(t_minishell *ms, char **acc)
{
	if (!acc || *acc == NULL)
		return (step_eof_main());
	return (step_eof_more(ms, acc));
}

/*
** 函数作用：
**   单步处理一次：读一行 -> 拼接 -> 交给 repl_run_acc。
** 参数：
**   ms：全局上下文
**   acc：累计输入（可能在 run 后被 free 并置 NULL）
** 返回：
**   1 表示退出；0 表示继续
*/
int	repl_step(t_minishell *ms, char **acc)
{
	char	*line;
	int		ok;

	line = NULL;
	ok = 0;
	line = repl_read(*acc);
	if (!line)
		return (step_handle_eof(ms, acc));
	ok = repl_join(acc, line);
	if (ok == 0)
		return (step_eof_more(ms, acc));
	repl_run_acc(ms, acc);
	return (0);
}
