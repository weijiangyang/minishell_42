/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_set.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 14:01:46 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 14:01:49 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h"

static t_env	*create_env_var(const char *key, const char *value)
{
	t_env	*new_var;
	char	*new_key;
	char	*new_value;

	new_key = ft_strdup(key);
	new_value = ft_strdup(value);
	if (!new_key || !new_value)
	{
		free(new_key);
		free(new_value);
		return (NULL);
	}
	new_var = malloc(sizeof(t_env));
	if (!new_var)
	{
		free(new_key);
		free(new_value);
		return (NULL);
	}
	new_var->key = new_key;
	new_var->value = new_value;
	new_var->next = NULL;
	return (new_var);
}

static void	add_env_var(t_env **env, t_env *new_var)
{
	t_env	*curr;

	if (!new_var)
		return ;
	if (!*env)
	{
		*env = new_var;
		return ;
	}
	curr = *env;
	while (curr->next)
		curr = curr->next;
	curr->next = new_var;
}

void	env_set(t_env **env, const char *key, const char *value)
{
	t_env	*var;
	char	*tmp;

	var = find_env_var(*env, key);
	if (var)
	{
		tmp = ft_strdup(value);
		if (!tmp)
			return ;
		free(var->value);
		var->value = tmp;
	}
	else
	{
		add_env_var(env, create_env_var(key, value));
	}
}
