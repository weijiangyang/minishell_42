/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:56 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:17:57 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

static int	env_len(t_env *env)
{
	int	len;

	len = 0;
	while (env)
	{
		len++;
		env = env->next;
	}
	return (len);
}

static char	*env_to_str(t_env *env)
{
	char	*key_value;
	char	*result;

	key_value = ft_strjoin(env->key, "=");
	if (!key_value)
		return (NULL);
	result = ft_strjoin(key_value, env->value);
	free(key_value);
	return (result);
}

void	change_envp(t_env *env, char ***envp)
{
	int		i;
	t_env	*tmp;
	int		len;

	if (*envp != NULL)
	{
		free_char_matrix(*envp);
		*envp = NULL;
	}
	len = env_len(env);
	*envp = malloc(sizeof(char *) * (len + 1));
	if (!*envp)
		return ;
	tmp = env;
	i = 0;
	while (tmp)
	{
		(*envp)[i] = env_to_str(tmp);
		tmp = tmp->next;
		i++;
	}
	(*envp)[i] = NULL;
}

void	print_env(t_env *env)
{
	while (env)
	{
		if (env->value)
		{
			ft_putstr_fd(env->key, STDOUT_FILENO);
			ft_putstr_fd("=", STDOUT_FILENO);
			ft_putstr_fd(env->value, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		env = env->next;
	}
}

t_env	*find_env_var(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}
