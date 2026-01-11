/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:05 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

/* 返回 1 表示这条命令里真的出现过 heredoc 操作符 <<（不在引号里） */
static int lexer_has_heredoc(t_lexer *lx)
{
	while (lx)
	{
		if (lx->tokentype == TOK_HEREDOC)
			return (1);
		lx = lx->next;
	}
	return (0);
}

/* 只复制第一行（遇到 '\n' 就截断） */
static char *dup_first_line(const char *s)
{
	size_t i;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	return (ft_substr(s, 0, i));
}

/* 作用：把新读到的一行接在之前没写完的命令后面。 */
int repl_join(char **acc, char *line)
{
	char *tmp;

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
static void run_drop_acc(t_minishell *ms, char **acc, int err_code)
{
	if (err_code != 0)
		ms->last_exit_status = err_code;
	repl_free_acc(acc);
	ms->raw_line = NULL;
	ms->lexer_need_more = 0;
	ms->lexer_unclosed_quote = 0;
}

static void run_one_cmd(t_minishell *ms)
{
	t_ast *root;
	t_lexer *tmp;

	tmp = ms->lexer;
	root = parse_cmdline(&tmp, ms);
	if (!root)
	{
		ms->last_exit_status = 2;
		clear_list(&ms->lexer);
		return;
	}
	ms->cur_t_ast = root;
	if (!prepare_heredocs(root, ms))
	{
		ms->cur_t_ast = NULL;
		free_ast(root);
		clear_list(&ms->lexer);
		return;
	}
	change_envp(ms->env, &ms->envp);
	expander_t_ast(ms, root);
	exec_t_ast(ms, root);
	ms->cur_t_ast = NULL;
	free_ast(root);
	clear_list(&ms->lexer);
}

/* 作用：判断命令是否写完，完整就去执行，不完整就等下一行。 */
void repl_run_acc(t_minishell *ms, char **acc)
{
	int lex_ret;
	char *hist;

	if (!repl_has_text(*acc))
		return (run_drop_acc(ms, acc, 0));
	ms->raw_line = *acc;
	lex_ret = handle_lexer(ms);
	if (lex_ret == LEX_NEED_MORE)
	{
		ms->raw_line = NULL;
		return;
	}
	if (lex_ret != LEX_OK)
		return (run_drop_acc(ms, acc, 2));
	if (isatty(STDIN_FILENO) && repl_has_text(*acc))
	{
		hist = NULL;
		if (lexer_has_heredoc(ms->lexer))
			hist = dup_first_line(*acc);
		else
			hist = ft_strdup(*acc);
		if (hist)
		{
			add_history(hist);
			free(hist);
		}
	}
	ms->raw_line = NULL;
	repl_free_acc(acc);
	run_one_cmd(ms);
	run_drop_acc(ms, acc, 0);
}

/* 作用：完全释放存命令的内存，防止泄露。 */
void repl_free_acc(char **acc)
{
	if (!acc)
		return;
	if (*acc)
		free(*acc);
	*acc = NULL;
}
