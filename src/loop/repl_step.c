/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_step.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:45 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/28 02:38:02 by yzhang2          ###   ########.fr       */
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

static int step_eof_main(void)
{
	if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
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
static int step_eof_more(t_minishell *ms, char **acc)
{
	char q;

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
static int step_handle_eof(t_minishell *ms, char **acc)
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
int repl_step(t_minishell *ms, char **acc)
{
	char *line;

	// 在读取前，如果 g_status 是 130，说明是上一行刚按了 Ctrl+C
	// 但通常我们关心的是“当前这一行”按下的 Ctrl+C
	line = repl_read(*acc);

	// 重点：readline 返回后立即检查全局变量
	// --- 修复逻辑开始 ---
	if (g_signal == SIGINT)
	{
		ms->last_exit_status = 130;
		g_signal = 0; // 重置全局变量
		if (line)
			free(line);
		// 如果在续行模式被中断，也要清理 acc
		if (acc && *acc)
		{
			free(*acc);
			*acc = NULL;
		}
		return (0); // 重新开始循环，此时状态码已经是 130
	}

	if (!line)
		return (step_handle_eof(ms, acc));

	int ok;
	ok = repl_join(acc, line); 
	free(line);
	line = NULL;
	if (ok == 0)
		return (step_eof_more(ms, acc));
	repl_run_acc(ms, acc);
	fflush(stdout);
	return (0);
}
