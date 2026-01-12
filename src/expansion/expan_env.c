/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:34:06 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 03:26:50 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "expander.h"
#include "minishell.h"

int	is_name_start(int c)
{
	return (ft_isalpha(c) || c == '_');
}

int	is_name_char(int c)
{
	return (ft_isalnum(c) || c == '_');
}

int	var_len(const char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (0);
	if (!is_name_start((unsigned char)s[i]))
		return (0);
	while (s[i] && is_name_char((unsigned char)s[i]))
		i++;
	return (i);
}

char	*env_value_dup(t_minishell *minishell, const char *name, int len)
{
	t_env	*cur;

	if (!minishell || !name || len <= 0)
		return (ft_strdup(""));
	cur = minishell->env;
	while (cur)
	{
		if (cur->key && (int)ft_strlen(cur->key) == len
			&& ft_strncmp(cur->key, name, len) == 0)
		{
			if (cur->value)
				return (ft_strdup(cur->value));
			return (ft_strdup(""));
		}
		cur = cur->next;
	}
	return (ft_strdup(""));
}
