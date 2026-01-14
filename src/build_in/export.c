/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:12 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/12 11:43:47 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

static int	parse_export_arg(char *arg, char **key, char **value)
{
	char	*equal;

	equal = ft_strchr(arg, '=');
	if (equal)
	{
		*key = ft_substr(arg, 0, equal - arg);
		*value = ft_strdup(equal + 1);
		if (!*key || !*value)
		{
			free(*key);
			free(*value);
			return (-1);
		}
	}
	else
	{
		*key = ft_strdup(arg);
		*value = NULL;
		if (!*key)
			return (-1);
	}
	return (0);
}

static void	update_env_var(t_env **env, char *key, char *value)
{
	t_env	*existing;

	existing = find_env_var(*env, key);
	if (existing)
	{
		if (value)
		{
			free(existing->value);
			existing->value = value;
		}
		free(key);
	}
	else
		env_add_back(env, env_new(key, value));
}

static int	export_one(char *arg, t_env **env)
{
	char	*key;
	char	*value;

	if (parse_export_arg(arg, &key, &value) == -1)
		return (1);
	if (!is_valid_identifier(key))
	{
		ms_put3("minishell: export: `", key, "': not a valid identifier\n");
		free(key);
		free(value);
		return (1);
	}
	update_env_var(env, key, value);
	return (0);
}

int	builtin_export(char **argv, t_env **env)
{
	int	status;
	int	i;

	status = 0;
	i = 1;
	if (!argv[1])
	{
		print_export(*env);
		return (0);
	}
	while (argv[i])
	{
		if (export_one(argv[i], env))
			status = 1;
		i++;
	}
	return (status);
}
