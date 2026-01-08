/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:05 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/29 18:02:28 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

/*
** 函数作用：
**   命令执行完之后，清理当前 acc（累计输入），并重置 lexer 状态。
** err_code：
**   - 0：正常清空
**   - 2：语法/词法错误（和 bash 一样返回 2）
*/
static void	run_drop_acc(t_minishell *ms, char **acc, int err_code)
{
	if (err_code != 0)
		ms->last_exit_status = err_code;
	repl_free_acc(acc);
	ms->raw_line = NULL;
	ms->lexer_need_more = 0;
	ms->lexer_unclosed_quote = 0;
}

/*
** 函数作用：
**   真正执行一条“已经完成”的命令：
**   lexer -> parser -> expander -> exec -> free
*/
static void	run_one_cmd(t_minishell *ms)
{
	ast		*root;
	t_lexer	*tmp;

	tmp = ms->lexer;
	root = parse_cmdline(&tmp, ms);
	if (!root)
	{
		if (ms->parse_status == PARSE_OK || ms->parse_status == PARSE_INCOMPLETE_INPUT)
			ms->last_exit_status = 0;
		else
			ms->last_exit_status= 2;
		clear_list(&ms->lexer);
		return ;
	}
	prepare_heredocs(root, ms);
	expander_ast(ms, root);
	exec_ast(ms, root);
	free_ast(root);
	clear_list(&ms->lexer);
}

/*
** 函数作用：
**   释放 acc（累计输入缓冲区），并把指针置为 NULL。
** 为什么要这样写：
**   - 置 NULL 可以防止“二次 free”这种常见 bug。
*/
void	repl_free_acc(char **acc)
{
	if (!acc)
		return ;
	if (*acc)
		free(*acc);
	*acc = NULL;
}


/*
** 函数作用：
**   拿着 acc 去做 lexer/parse/exec。
** 注意：
**   - 如果 lexer 发现“引号没闭合”，会返回 LEX_NEED_MORE；
**     这时不执行，只保留 acc，下一轮用 \"> \" 继续读。
*/
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
	run_one_cmd(ms);
	run_drop_acc(ms, acc, 0);
}
