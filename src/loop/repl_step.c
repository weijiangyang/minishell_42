/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_step.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:45 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/15 17:52:20 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

static int	handle_interruption(t_minishell *ms, char **acc, char *line)
{
	ms->last_exit_status = 130;
	g_signal = 0;
	if (!line)
	{
		ms_clear(ms);
		ms->should_exit = 1;
		if (acc && *acc)
		{
			free(*acc);
			*acc = NULL;
		}
		return (0);
	}
	if (line)
		free(line);
	if (acc && *acc)
	{
		free(*acc);
		*acc = NULL;
	}
	return (0);
}

static int	step_eof_more(t_minishell *ms, char **acc)
{
	char	q;

	q = '"';
	if (ms && ms->lexer_unclosed_quote)
		q = ms->lexer_unclosed_quote;
	ms_err_eof_quote(q);
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

static int	step_handle_eof(t_minishell *ms, char **acc)
{
	if (!acc || *acc == NULL)
	{
		if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
			ft_putstr_fd("exit\n", STDOUT_FILENO);
		return (1);
	}
	return (step_eof_more(ms, acc));
}

int	repl_step(t_minishell *ms, char **acc)
{
	char	*line;
	int		ok;

	line = repl_read(*acc);
	if (g_signal == SIGINT)
		return (handle_interruption(ms, acc, line));
	if (!line)
		return (step_handle_eof(ms, acc));
	ok = repl_join(acc, line);
	free(line);
	if (ok == 0)
		return (step_eof_more(ms, acc));
	repl_run_acc(ms, acc);
	return (0);
}
