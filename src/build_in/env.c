/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:46 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:16:49 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

t_env	*env_new(char *key, char *value)
{
	t_env	*new_env;

	new_env = malloc(sizeof(t_env));
	if (!new_env)
		return (NULL);
	new_env->key = key;
	new_env->value = value;
	new_env->next = NULL;
	return (new_env);
}

void	env_add_back(t_env **env, t_env *new_env)
{
	t_env	*curr;

	if (!env || !new_env)
		return ;
	if (*env == NULL)
	{
		*env = new_env;
		return ;
	}
	curr = *env;
	while (curr->next)
		curr = curr->next;
	curr->next = new_env;
}

t_env	*init_env(char **envp)
{
	t_env	*env;
	char	*equal;
	char	*key;
	char	*value;
	int		i;

	env = NULL;
	i = 0;
	while (envp[i])
	{
		equal = ft_strchr(envp[i], '=');
		if (equal)
		{
			key = ft_substr(envp[i], 0, equal - envp[i]);
			value = ft_strdup(equal + 1);
			env_add_back(&env, env_new(key, value));
		}
		i++;
	}
	return (env);
}

int	builtin_env(char **argv, t_env *env)
{
	if (argv[1])
	{
		ms_put3("env: ", argv[1], " No such file or directory\n");
		return (127);
	}
	print_env(env);
	return (0);
}
