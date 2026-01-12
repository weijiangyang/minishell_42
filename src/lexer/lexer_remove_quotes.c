/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_remove_quotes.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 05:16:37 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:23:49 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

static void	copy_quoted_block(const char *s, char *res, t_index *idx,
	char quote)
{
	if (s[idx->i] == quote)
	{
		(idx->i)++;
		while (s[idx->i] && s[idx->i] != quote)
			res[(idx->j)++] = s[(idx->i)++];
		if (s[idx->i] == quote)
			(idx->i)++;
	}
}

static size_t	copy_content_no_quotes(const char *s, char *res)
{
	t_index	idx;

	idx.i = 0;
	idx.j = 0;
	while (s[idx.i])
	{
		if (s[idx.i] == '\'')
			copy_quoted_block(s, res, &idx, '\'');
		else if (s[idx.i] == '"')
			copy_quoted_block(s, res, &idx, '"');
		else
			res[idx.j++] = s[idx.i++];
	}
	return (idx.j);
}

static int	check_and_set_flags(const char *s, int *q_single, int *q_double)
{
	size_t	i;

	i = 0;
	*q_single = 0;
	*q_double = 0;
	while (s[i])
	{
		if (s[i] == '\'')
			*q_single = 1;
		if (s[i] == '"')
			*q_double = 1;
		i++;
	}
	if (*q_single == 0 && *q_double == 0)
		return (0);
	return (1);
}

char	*remove_quotes_flag(const char *s, int *had_q, int *q_s, int *q_d)
{
	size_t	len;
	size_t	j;
	char	*res;

	res = NULL;
	*had_q = 0;
	if (!s)
	{
		*q_s = 0;
		*q_d = 0;
		return (NULL);
	}
	if (check_and_set_flags(s, q_s, q_d) == 0)
		return (NULL);
	len = ft_strlen(s);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	j = copy_content_no_quotes(s, res);
	res[j] = '\0';
	*had_q = 1;
	return (res);
}
