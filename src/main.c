/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:20:01 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/28 02:34:55 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/build_in.h"
#include "../include/exec.h"
#include "../include/minishell.h"
#include "../include/repl.h"
#include <stdio.h>
#include <unistd.h>

/* 作用：释放 char**（以 NULL 结尾的字符串数组）。 */
static void	free_matrix(char **m)
{
	int	i;

	i = 0;
	if (!m)
		return ;
	while (m[i])
	{
		free(m[i]);
		i = i + 1;
	}
	free(m);
}

/* 作用：把 minishell 的上下文字段初始化为“安全的默认值”。 */
static void	ms_reset(t_minishell *ms)
{
	if (!ms)
		return ;
	ms->lexer = NULL;
	ms->env = NULL;
	ms->raw_line = NULL;
	ms->n_pipes = 0;
	ms->last_exit_status = 0;
	ms->lexer_unclosed_quote = '\0';
	ms->lexer_need_more = 0;
	ms->envp = NULL;
	ms->paths = NULL;
}

/* 作用：初始化环境（env 链表 + envp 数组），供 expand/exec 使用。 */
static int	ms_init(t_minishell *ms, char **envp)
{
	ms_reset(ms);
	ms->env = init_env(envp);
	change_envp(ms->env, &ms->envp);
	if (!ms->envp)
		return (1);
	return (0);
}

/* 作用：退出前清理资源。 */
static void	ms_destroy(t_minishell *ms)
{
	if (!ms)
		return ;
	if (ms->lexer)
		clear_list(&ms->lexer);
	free_paths(ms);
	free_matrix(ms->envp);
	ms->envp = NULL;
	free_env(ms->env);
	ms->env = NULL;
}

/*
** 主函数作用：
** 1) 初始化 minishell 上下文（环境变量、状态码等）；
** 2) 根据是否是交互终端，设置提示符相关信号；
** 3) 进入 REPL 循环（读一行 -> 解析 -> 执行）。
*/
int	main(int argc, char **argv, char **envp)
{
	t_minishell	ms;
	int			ret;

	(void)argc;
	(void)argv;
	ret = 0;
	if (ms_init(&ms, envp) != 0)
		return (1);
	if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
		setup_prompt_signals();
	repl_loop(&ms);
	ret = ms.last_exit_status;
	ms_destroy(&ms);
	return (ret);
}
