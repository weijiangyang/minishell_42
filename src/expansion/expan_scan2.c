/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_scan2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:32:30 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 19:49:42 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

static void	toggle_quote_state(const char c, enum e_qstate *q)
{
	if (c == '\'' && *q != Q_DQ)
	{
		if (*q == Q_SQ)
			*q = Q_NONE;
		else
			*q = Q_SQ;
	}
	else if (c == '\"' && *q != Q_SQ)
	{
		if (*q == Q_DQ)
			*q = Q_NONE;
		else
			*q = Q_DQ;
	}
}

static int	append_char(const char c, char **out)
{
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	*out = str_join_free(*out, buf);
	return (1);
}

char	*expand_all(t_minishell *minishell, const char *str)
{
	int				i;
	char			*out;
	enum e_qstate	q;
	t_exp_data		data;

	i = 0;
	out = NULL;
	q = Q_NONE;
	if (!str)
		return (NULL);
	out = ft_strdup("");
	if (!out)
		return (NULL);
	data.minishell = minishell;
	data.out = &out;
	while (str[i])
	{
		toggle_quote_state(str[i], &q);
		if (str[i] == '$')
			i += scan_expand_one(&data, str, i, q);
		else
			i += append_char(str[i], &out);
	}
	return (out);
}
