/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:53:04 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 18:40:10 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"

/*
** 作用：跳过从某个位置开始的所有空格。
*/
int	skip_spaces(char *str, int i)
{
	int	j;

	j = 0;
	while (check_space(str[i + j]))
		j++;
	return (j);
}

/*
** 作用：如果发现引号没写完，就记录状态并告诉系统需要更多输入。
*/
static int	process_unclosed(t_minishell *ms, char quote)
{
	ms->lexer_unclosed_quote = quote;
	ms->lexer_need_more = 1;
	clear_list(&ms->lexer);
	return (LEX_NEED_MORE);
}

/*
** 作用：根据当前字符是特殊符号还是普通文字，选择不同的处理方式。
*/
static int	process_segment(t_minishell *ms, int i)
{
	int		step;
	char	quote;

	quote = '\0';
	if (is_token((unsigned char)ms->raw_line[i]))
		step = handle_token(ms->raw_line, i, &ms->lexer);
	else
		step = handle_word(ms->raw_line, i, &ms->lexer, &quote);
	if (step == LEX_UNCLOSED_QUOTE)
		return (process_unclosed(ms, quote));
	if (step < 0)
	{
		clear_list(&ms->lexer);
		return (0);
	}
	return (step);
}

/*
** 作用：这是切分命令的总入口，把一整行命令拆成一个一个的“零件”。
*/
int	handle_lexer(t_minishell *ms)
{
	int	i;
	int	step;

	if (!ms || !ms->raw_line)
		return (0);
	i = 0;
	ms->lexer_unclosed_quote = '\0';
	ms->lexer_need_more = 0;
	while (ms->raw_line[i])
	{
		if (detect_signal(&ms->lexer))
			return (0);
		i += skip_spaces(ms->raw_line, i);
		if (ms->raw_line[i] == '\0')
			break ;
		step = process_segment(ms, i);
		if (step <= 0)
			return (step);
		i += step;
	}
	return (1);
}
