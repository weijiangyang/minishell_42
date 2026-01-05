/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_set.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 14:01:46 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 14:01:49 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/build_in.h"
#include "../../include/minishell.h"


// 设置或更新环境变量
static t_env *create_env_var(const char *key, const char *value)
{
	t_env *new_var;
	char *new_key;
	char *new_value;

	new_key = strdup(key);
	new_value = strdup(value);
	if (!new_key || !new_value)
	{
		perror("strdup");
		free(new_key);
		free(new_value);
		exit(EXIT_FAILURE);
	}
	new_var = malloc(sizeof(t_env));
	if (!new_var)
	{
		perror("malloc");
		free(new_key);
		free(new_value);
		exit(EXIT_FAILURE);
	}
	new_var->key = new_key;
	new_var->value = new_value;
	new_var->next = NULL;
	return new_var;
}

static void add_env_var(t_env **env, t_env *new_var)
{
	t_env *last;

	if (!*env)
	{
		*env = new_var;
		return;
	}
	last = *env;
	while (last->next)
		last = last->next;
	last->next = new_var;
}

void env_set(t_env **env, const char *key, const char *value)
{
	t_env *var;

	var = find_env_var(*env, key);
	if (var)
	{
		free(var->value);
		var->value = strdup(value);
		if (!var->value)
		{
			perror("strdup");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		add_env_var(env, create_env_var(key, value));
	}
}
