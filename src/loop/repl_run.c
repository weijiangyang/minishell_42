/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:05 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/22 15:18:58 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "repl.h"


/*
** 函数作用：
**   清理 lexer 生成的 token 链表，避免内存泄漏。
** 参数：
**   ms：全局上下文
*/
static void	run_clear_lexer(t_minishell *ms)
{
	if (!ms)
		return ;
	if (ms->lexer)
		clear_list(&ms->lexer);
}

/*
** 函数作用：
**   parse + expand + exec 一条完整命令。
** 参数：
**   ms：全局上下文（ms->lexer 里是 token 链表）
*/
static void	run_parse_exec(t_minishell *ms)
{
	ast		*root;
	t_lexer	*cur;

	root = NULL;
	cur = NULL;
	if (!ms)
		return ;
	cur = ms->lexer;
	root = parse_cmdline(&cur, ms);
	if (root)
	{
		expander_ast(ms, root);
		exec_ast(ms, root);
		free_ast(root);
	}
}

/*
** 函数作用：
**   执行结束或报错时，释放 acc 并重置 raw_line。
** 参数：
**   ms：全局上下文
**   acc：累计输入
**   code：要写入的退出码（你们字段名若不同，请改这里）
*/
static void	run_drop_acc(t_minishell *ms, char **acc, int code)
{
	if (ms)
		ms->last_exit_status = code;
	if (acc && *acc)
		free(*acc);
	if (acc)
		*acc = NULL;
	if (ms)
		ms->raw_line = NULL;
}

/*
** 函数作用：
**   把 acc 交给 lexer：
**   - LEX_NEED_MORE：保留 acc 等待续行
**   - LEX_OK：add_history + parse/expand/exec + 清理
**   - LEX_ERR：丢弃 acc，设置状态码
** 参数：
**   ms：全局上下文
**   acc：累计输入（函数可能 free 并置 NULL）
*/
void	repl_run_acc(t_minishell *ms, char **acc)
{
	int	lex_ret;

	lex_ret = 0;
	if (!ms || !acc || !*acc)
		return ;
	ms->raw_line = *acc;
	lex_ret = handle_lexer(ms);
	if (lex_ret == LEX_NEED_MORE)
		return (ms->raw_line = NULL, (void)0);
	if (lex_ret != LEX_OK)
		return (run_drop_acc(ms, acc, 2));
	if (repl_has_text(*acc))
		add_history(*acc);
	run_parse_exec(ms);
	run_clear_lexer(ms);
	run_drop_acc(ms, acc, ms->last_exit_status);
}
