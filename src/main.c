/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:20:01 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

static void	ms_report_error(const char *s)
{
	int	i;

	if (!s)
		return ;
	i = 0;
	while (s[i])
		i++;
	write(STDERR_FILENO, s, i);
}

static int	ms_initial_env(t_minishell *ms, char **envp)
{
	ms->env = init_env(envp);
	if (!ms->env)
		return (0);
	ms->envp = NULL;
	change_envp(ms->env, &ms->envp);
	if (!ms->envp)
		return (0);
	return (1);
}

static int	ms_initial_system(t_minishell *ms, char **envp)
{
	ms->env = NULL;
	ms->envp = NULL;
	ms->paths = NULL;
	ms->lexer = NULL;
	ms->cur_ast = NULL;
	ms->raw_line = NULL;
	ms->should_exit = 0;
	ms->last_exit_status = 0;
	ms->lexer_need_more = 0;
	ms->lexer_unclosed_quote = 0;
	ms->heredoc_quoted = 0;
	ms->heredoc_write_fd = -1;
	if (!ms_initial_env(ms, envp))
		return (0);
	if (ensure_paths_ready(ms) != 0)
		return (0);
	return (1);
}

static int	check_sign(void)
{
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	ms;

	(void)argv;
	if (argc != 1)
	{
		ms_report_error("minishell: no argument allowed\n");
		return (1);
	}
	rl_event_hook = check_sign;
	setup_prompt_signals();
	if (!ms_initial_system(&ms, envp))
	{
		ms_report_error("minishell: init failed\n");
		ms_clear(&ms);
		return (1);
	}
	repl_loop(&ms);
	ms_clear(&ms);
	return (ms.last_exit_status);
}
