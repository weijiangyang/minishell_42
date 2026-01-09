/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:05 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 01:07:46 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

/* 作用：把新读到的一行接在之前没写完的命令后面。 */
int	repl_join(char **acc, char *line)
{
	char	*tmp;

	if (!acc || !line)
		return (0);
	if (!*acc)
	{
		*acc = ft_strdup(line);
		return (*acc != NULL);
	}
	tmp = ft_strjoin(*acc, "\n");
	free(*acc);
	*acc = tmp;
	if (!*acc)
		return (0);
	tmp = ft_strjoin(*acc, line);
	free(*acc);
	*acc = tmp;
	return (*acc != NULL);
}

/* 作用：执行完后打扫战场，把积累的输入清空。 */
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
	ast		*root;
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
	expander_ast(ms, root);
	exec_ast(ms, root);
	ms->cur_ast = NULL;
	free_ast(root);
	clear_list(&ms->lexer);
}

/* 作用：判断命令是否写完，完整就去执行，不完整就等下一行。 */
void	repl_run_acc(t_minishell *ms, char **acc)
{
	int	lex_ret;

	if (!repl_has_text(*acc))
		return (run_drop_acc(ms, acc, 0));
	ms->raw_line = *acc;
	lex_ret = handle_lexer(ms);
	if (lex_ret == LEX_NEED_MORE)
	{
		ms->raw_line = NULL;
		return ;
	}
	if (lex_ret != LEX_OK)
		return (run_drop_acc(ms, acc, 2));
	if (isatty(STDIN_FILENO) && repl_has_text(*acc))
		add_history(*acc);
	ms->raw_line = NULL;
	repl_free_acc(acc);
	run_one_cmd(ms);
	run_drop_acc(ms, acc, 0);
}

/* 作用：完全释放存命令的内存，防止泄露。 */
void	repl_free_acc(char **acc)
{
	if (!acc)
		return ;
	if (*acc)
		free(*acc);
	*acc = NULL;
}
