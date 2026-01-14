/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:05 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:36:42 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

static void	run_drop_acc(t_minishell *ms, char **acc, int err_code)
{
	if (err_code != 0)
		ms->last_exit_status = err_code;
	repl_free_acc(acc);
	ms->raw_line = NULL;
	ms->lexer_need_more = 0;
	ms->lexer_unclosed_quote = 0;
}

static void	run_one_cmd(t_minishell *ms)
{
	t_ast	*root;
	t_lexer	*tmp;

	tmp = ms->lexer;
	root = parse_cmdline(&tmp, ms);
	if (!root)
	{
		ms->last_exit_status = 2;
		clear_list(&ms->lexer);
		return ;
	}
	ms->cur_ast = root;
	if (!prepare_heredocs(root, ms))
	{
		ms->cur_ast = NULL;
		free_ast(root);
		clear_list(&ms->lexer);
		return ;
	}
	change_envp(ms->env, &ms->envp);
	expander_t_ast(ms, root);
	exec_t_ast(ms, root);
	ms->cur_ast = NULL;
	free_ast(root);
	clear_list(&ms->lexer);
}

static void	repl_add_history(t_minishell *ms, char *acc)
{
	char	*hist;

	if (isatty(STDIN_FILENO) && repl_has_text(acc))
	{
		hist = NULL;
		if (lexer_has_heredoc(ms->lexer))
			hist = dup_first_line(acc);
		else
			hist = ft_strdup(acc);
		if (hist)
		{
			add_history(hist);
			free(hist);
		}
	}
}

void	repl_run_acc(t_minishell *ms, char **acc)
{
	int	lex_ret;

	if (!repl_has_text(*acc))
	{
		run_drop_acc(ms, acc, 0);
		return ;
	}
	ms->raw_line = *acc;
	lex_ret = handle_lexer(ms);
	if (lex_ret == LEX_NEED_MORE)
	{
		ms->raw_line = NULL;
		return ;
	}
	if (lex_ret != LEX_OK)
	{
		run_drop_acc(ms, acc, 2);
		return ;
	}
	repl_add_history(ms, *acc);
	ms->raw_line = NULL;
	repl_free_acc(acc);
	run_one_cmd(ms);
	run_drop_acc(ms, acc, 0);
}
