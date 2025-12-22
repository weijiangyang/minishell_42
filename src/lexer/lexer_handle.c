/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:53:04 by yzhang2           #+#    #+#             */
/*   Updated: 2025/11/10 19:09:17 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/lexer.h"

// 作用：跳过从 `i` 开始的连续空白。
// 参数：命令串、起点。
int	skip_spaces(char *str, int i)
{
	int	j;

	j = 0;
	while (is_space(str[i + j]))
		j++;
	return (j);
}

// 作用：处理中断：若处于 SIGINT 状态，清空已构建的词法链表并返回“被打断”。
// 参数：链表头地址。
// 逻辑：检测全局信号（你项目）；如触发→`clear_list(list)`→复位→返回 1，否则 0。
static int	check_sigint(t_lexer **list)
{
	if (g_signal == SIGINT)
	{
		clear_list(list);
		g_signal = 0;
		return (1);
	}
	return (0);
}

// 作用：对 `minishell->raw_line` 执行整行词法拆分。
// 参数：全局上下文（含输入字符串 `raw_line` 与输出链表 `lexer`）。
// 实现逻辑：
//   * 初始化索引 `i`，循环直到 `raw_line[i]=='\0'`；
//   * 先 `skip_spaces`；
//   * 若 `is_token(raw_line[i])` 为真 → `j = handle_token(...)`；
// 否则 `j = handle_word(...)`；
//   * 若 `j < 0`（如引号错误/内存失败）→ `clear_list(&minishell->lexer)` 并返回 `0`（失败）；
//   * 否则 `i += j` 继续；
//   * 结束返回 `1`（成功）。
int	handle_lexer(t_minishell *minishell)
{
	int	i;
	int	j;
	char	unclosed;

	if (!minishell || !minishell->raw_line)
		return (0);
	i = 0;
	/* 初始化上次可能存在的未闭合标志 */
	minishell->lexer_unclosed_quote = '\0';
	minishell->lexer_need_more = 0;
	while (minishell->raw_line[i])
	{
		if (check_sigint(&minishell->lexer))
			return (0);
		i += skip_spaces(minishell->raw_line, i);
		if (minishell->raw_line[i] == '\0')
			break ;
		unclosed = '\0';
		if (is_token((unsigned char)minishell->raw_line[i]))
			j = handle_token(minishell->raw_line, i, &minishell->lexer);
		else
			j = handle_word(minishell->raw_line, i, &minishell->lexer, &unclosed);
		/* 若发现未闭合引号（由 handle_word 报告），把状态记入 minishell 并返回 NEED_MORE */
		if (j == LEX_UNCLOSED_QUOTE)
		{
			minishell->lexer_unclosed_quote = unclosed;
			minishell->lexer_need_more = 1;
			/* 清除已构建但不完整的词法链表（不加入 history） */
			clear_list(&minishell->lexer);
			return (LEX_NEED_MORE);
		}
		if (j < 0)
		{
			clear_list(&minishell->lexer);
			return (0);
		}
		i += j;
	}
	return (1);
}
