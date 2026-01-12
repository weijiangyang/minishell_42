/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_heredoc_val.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:20:22 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

char	*search_env(t_minishell *ms, const char *key)
{
	t_env	*node;

	if (!ms || !key)
		return ("");
	node = ms->env;
	while (node)
	{
		if (ft_strcmp(node->key, key) == 0)
			return (node->value);
		node = node->next;
	}
	return ("");
}

static char	*handle_status(t_minishell *ms, char *result, int *step)
{
	char	*value;

	value = ft_itoa(ms->last_exit_status);
	if (!value)
		return (result);
	result = ft_strjoin_free(result, value);
	free(value);
	*step = 1;
	return (result);
}

static char	*handle_variable(t_minishell *ms, char *target, char *result,
								int *step)
{
	char	*key;
	char	*value;
	int		length;

	length = 0;
	while (target[length] && (ft_isalnum(target[length])
			|| target[length] == '_'))
		length++;
	key = ft_substr(target, 0, length);
	if (!key)
		return (result);
	value = search_env(ms, key);
	result = ft_strjoin_free(result, value);
	free(key);
	*step = length;
	return (result);
}

static char	*join_char(char *result, char c)
{
	char	buffer[2];

	buffer[0] = c;
	buffer[1] = '\0';
	return (ft_strjoin_free(result, buffer));
}

char	*expand_heredoc_vars(t_minishell *ms, char *source)
{
	char	*result;
	int		step;

	result = ft_strdup("");
	if (!result || !source)
		return (result);
	while (*source)
	{
		step = 0;
		if (*source == '$' && *(source + 1) == '?')
			result = handle_status(ms, result, &step);
		else if (*source == '$' && (ft_isalpha(*(source + 1))
				|| *(source + 1) == '_'))
			result = handle_variable(ms, source + 1, result, &step);
		else if (*source == '$')
			result = ft_strjoin_free(result, "$");
		else
			result = join_char(result, *source);
		if (step > 0)
			source += step + 1;
		else
			source++;
	}
	return (result);
}
