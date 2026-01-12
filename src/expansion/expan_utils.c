/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 19:39:04 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 02:57:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

int	word_has_quotes(const char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] == '\'' || s[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

size_t	equal_sign(char *entry)
{
	size_t	i;

	if (!entry)
		return (0);
	i = 0;
	while (entry[i] && entry[i] != '=')
		i++;
	return (i);
}

char	*strip_all_quotes_dup(const char *s, int *had_q, int *q_s, int *q_d)
{
	char	*clean;

	if (!s)
		return (NULL);
	clean = remove_quotes_flag(s, had_q, q_s, q_d);
	if (!clean)
		clean = ft_strdup(s);
	return (clean);
}
