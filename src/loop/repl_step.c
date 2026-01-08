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
