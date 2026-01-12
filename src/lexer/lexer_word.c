/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:53:56 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:24:44 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

int	check_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static int	verify_quote_status(char *str, int i, char *out_unclosed)
{
	int	q_len;

	q_len = match_quotes(i, str, '"');
	if (q_len == -1)
	{
		if (out_unclosed)
			*out_unclosed = '"';
		return (LEX_UNCLOSED_QUOTE);
	}
	if (q_len > 0)
		return (q_len);
	q_len = match_quotes(i, str, '\'');
	if (q_len == -1)
	{
		if (out_unclosed)
			*out_unclosed = '\'';
		return (LEX_UNCLOSED_QUOTE);
	}
	return (q_len);
}

static int	calc_word_len(char *str, int start_i, char *out_unclosed)
{
	int	j;
	int	q_len;

	j = 0;
	if (out_unclosed)
		*out_unclosed = '\0';
	while (str[start_i + j] && !(is_token((unsigned char)str[start_i + j])))
	{
		q_len = verify_quote_status(str, start_i + j, out_unclosed);
		if (q_len == LEX_UNCLOSED_QUOTE)
			return (LEX_UNCLOSED_QUOTE);
		j += q_len;
		if (!str[start_i + j] || check_space(str[start_i + j]))
			break ;
		if (str[start_i + j] == '"' || str[start_i + j] == '\'')
			continue ;
		j++;
	}
	return (j);
}

static int	process_word_data(char *substr, t_token_info *info)
{
	char	*clean;

	if (!info)
		return (0);
	clean = remove_quotes_flag(substr, &info->had_quotes, &info->quoted_single,
			&info->quoted_double);
	info->raw = substr;
	if (!clean)
	{
		info->clean = substr;
		info->had_quotes = 0;
		info->quoted_single = 0;
		info->quoted_double = 0;
		return (1);
	}
	info->clean = clean;
	return (1);
}

int	handle_word(char *str, int i, t_lexer **list, char *out_unclosed)
{
	int				j;
	char			*substr;
	t_token_info	info;

	j = calc_word_len(str, i, out_unclosed);
	if (j == LEX_UNCLOSED_QUOTE || j <= 0)
		return (j);
	substr = ft_substr(str, i, j);
	if (!substr)
		return (-1);
	process_word_data(substr, &info);
	if (finalize_word_node(&info, list) < 0)
		return (-1);
	return (j);
}
