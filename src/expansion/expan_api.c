/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_api.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:34:25 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:36:42 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"

static char	*api_clean_dup(const char *s)
{
	char	*clean;
	int		had_q;
	int		q_s;
	int		q_d;

	had_q = 0;
	q_s = 0;
	q_d = 0;
	clean = remove_quotes_flag(s, &had_q, &q_s, &q_d);
	if (!clean)
		clean = ft_strdup(s);
	return (clean);
}

char	*expander_str(t_minishell *minishell, char *str)
{
	char	*tmp;
	char	*clean;

	if (!str)
		return (NULL);
	tmp = expand_all(minishell, str);
	if (!tmp)
	{
		free(str);
		return (NULL);
	}
	clean = api_clean_dup(tmp);
	free(tmp);
	free(str);
	return (clean);
}
