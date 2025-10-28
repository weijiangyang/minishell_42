/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 04:44:56 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/26 05:10:14 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 作用：对单字符进行 token 分类。
// 参数：`c` 字符。
// 逻辑：按字符返回 `PIPE` / `LESS` / `GREAT` / `WORD` 等
t_tokens	is_token(int c)
{
	if (c == '|')
		return (PIPE);
	if (c == '>')
		return (GREAT);
	if (c == '<')
		return (LESS);
	if (c == '&')
		return (AMP);
	if (c == ';')
		return (SEMI);
	if (c == '(')
		return (LPAREN);
	if (c == ')')
		return (RPAREN);
	return (0);
}

// 作用：初始化一次 token 解析的临时状态。
// 参数：`info`。
// 逻辑：置零/默认值，避免脏读。（函数体省略，职责由命名+调用点推定）
static void	init_token_info(t_token_info *info)
{
	info->clean = NULL;
	info->raw = NULL;
	info->had_quotes = 0;
	info->quoted_single = 0;
	info->quoted_double = 0;
}

// 作用：处理 `<<` / `>>` 等双字符、运算符，并入链表。
// 参数：当前已判定的单字符类型、下一个字符、临时信息、链表头。
// 逻辑：若可与下一字符组成复合 token，则设定为 `HEREDOC`/`APPEND` 等，
// 创建节点并返回消费长度；否则按单字符处理。
static int	handle_double_token(t_tokens token, int next_char,
		t_token_info *info, t_lexer **list)
{
	if (token == GREAT && is_token(next_char) == GREAT)
	{
		if (!add_node(info, GREAT_GREAT, list))
			return (-1);
		return (2);
	}
	if (token == LESS && is_token(next_char) == LESS)
	{
		if (!add_node(info, LESS_LESS, list))
			return (-1);
		return (2);
	}
	if (token == PIPE && is_token(next_char) == PIPE)
	{
		if (!add_node(info, OR_OR, list))
			return (-1);
		return (2);
	}
	if (token == AMP && is_token(next_char) == AMP)
	{
		if (!add_node(info, AND_AND, list))
			return (-1);
		return (2);
	}
	return (0);
}

// 作用：在 `str[i]` 解析一个符号类 token 并加入链表。
// 参数：命令串、起始下标、链表头。
// 逻辑：调用 `is_token` 与 `handle_double_token` 决定 1/2 字符长度，
// 填充 `t_token_info`→`add_node`→返回消费的字符数；失败返回负值。
int	handle_token(char *str, int i, t_lexer **list)
{
	t_tokens		token;
	int				next_char;
	t_token_info	info;
	int				res;

	init_token_info(&info);
	token = is_token((unsigned char)str[i]);
	next_char = (unsigned char)str[i + 1];
	res = handle_double_token(token, next_char, &info, list);
	if (res != 0)
		return (res);
	if (token)
	{
		if (!add_node(&info, token, list))
			return (-1);
		return (1);
	}
	return (0);
}

// 作用：匹配从 `i` 开始的一段**完整引号**并返回长度（未闭合返回 `-1`）。
// 参数：起点、源串、引号字符 `'` 或 `"`。
// 逻辑：从位置 i 开始：若 str[i] 不是目标引号，返回 0；若是，则向后扫描直到遇到同类闭合引号，
// 找到则返回包含首尾引号在内的总长度，未找到闭合引号返回 -1。
int	match_quotes(int i, char *str, char quote)
{
	int	j;

	j = 0;
	if (str[i + j] == quote)
	{
		j++;
		while (str[i + j] != quote && str[i + j])
			j++;
		if (str[i + j] == quote)
			j++;
		else
			return (-1);
	}
	return (j);
}
