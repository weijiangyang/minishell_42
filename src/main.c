/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:20:01 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/11 18:20:12 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

/*
** 作用：向屏幕报告错误。
*/
static void ms_report_error(const char *s)
{
	int i;

	i = 0;
	while (s[i])
		i++;
	write(2, s, i);
}

/*
** 作用：把电脑里的环境变量拿到程序里用。
*/
static int ms_initial_env(t_minishell *ms, char **envp)
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

/*
** 作用：给程序做准备工作，清空旧数据并设置新起点。
*/
static int ms_initial_system(t_minishell *ms, char **envp)
{
	ms->env = NULL;
	ms->envp = NULL;
	ms->paths = NULL;
	ms->lexer = NULL;
	ms->cur_t_ast = NULL;
	ms->raw_line = NULL;
	ms->should_exit = 0;
	ms->lt_ast_exit_status = 0;
	ms->lexer_need_more = 0;
	ms->lexer_unclosed_quote = 0;
	if (!ms_initial_env(ms, envp))
		return (0);
	if (ensure_paths_ready(ms) != 0)
		return (0);
	return (1);
}

/*
** 作用：程序的大门。负责检查参数、启动循环，最后打扫卫生。
*/
int main(int argc, char **argv, char **envp)
{
	t_minishell ms;

	(void)argv;
	if (argc != 1)
	{
		ms_report_error("minishell: no argument allowed\n");
		return (1);
	}
	setup_prompt_signals();
	if (!ms_initial_system(&ms, envp))
	{
		ms_report_error("minishell: init failed\n");
		ms_clear(&ms);
		return (1);
	}
	repl_loop(&ms);
	ms_clear(&ms);
	return (ms.lt_ast_exit_status);
}
