/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_step.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:45 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 19:20:43 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "../../include/repl.h"

/* 作用：当用户按了 Ctrl+C 时，负责清空当前正在写的半截命令。 */
static int	handle_interruption(t_minishell *ms, char **acc, char *line)
{
	ms->last_exit_status = 130;
	g_signal = 0;
	if (line)
		free(line);
	if (acc && *acc)
	{
		free(*acc);
		*acc = NULL;
	}
	return (0);
}

/* 作用：如果引号没写完就断掉了，给用户报个错。 */
static int	step_eof_more(t_minishell *ms, char **acc)
{
	char	q;

	// 1. 确定是哪种引号没闭合
	q = '"';
	if (ms && ms->lexer_unclosed_quote)
		q = ms->lexer_unclosed_quote;
	// 2. 调用我们自定义的报错函数代替 fprintf
	ms_err_eof_quote(q);
	// 3. 状态维护
	if (ms)
	{
		ms->last_exit_status = 258;
		ms->raw_line = NULL;
	}
	if (acc && *acc)
	{
		free(*acc);
		*acc = NULL;
	}
	return (0);
}

/* 作用：处理各种输入突然中止的情况。 */
static int	step_handle_eof(t_minishell *ms, char **acc)
{
	if (!acc || *acc == NULL)
	{
		if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
			printf("exit\n");
		return (1);
	}
	return (step_eof_more(ms, acc));
}

/* 作用：走一小步：读入一行，看看是执行还是继续读。 */
int	repl_step(t_minishell *ms, char **acc)
{
	char	*line;
	int		ok;

	line = repl_read(*acc);
	if (g_signal == SIGINT)
		return (handle_interruption(ms, acc, line));
	if (!line)
		return (step_handle_eof(ms, acc));

	if (!*acc)
	{
		ms->input_line = ft_strdup(line);
		(ms->lineno)++;
	}
	//printf("%d\n", ms->lineno);
	//printf("%s\n", ms->input_line);

	ok = repl_join(acc, line);
	free(line);
	if (ok == 0)
		return (step_eof_more(ms, acc));
	repl_run_acc(ms, acc);
	fflush(stdout);
	return (0);
}

