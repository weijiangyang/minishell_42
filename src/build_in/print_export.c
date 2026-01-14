/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_export.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 11:25:20 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/14 11:25:23 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

static int	env_size(t_env *env)
{
	int	i;

	i = 0;
	while (env)
	{
		i++;
		env = env->next;
	}
	return (i);
}

static t_env	**env_to_array(t_env *env)
{
	t_env	**arr;
	int		i;
	int		size;

	size = env_size(env);
	arr = malloc(sizeof(t_env *) * (size + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (env)
	{
		arr[i++] = env;
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}

static void	sort_env_array(t_env **arr)
{
	int		i;
	int		j;
	t_env	*tmp;

	i = 0;
	while (arr[i])
	{
		j = i + 1;
		while (arr[j])
		{
			if (ft_strcmp(arr[i]->key, arr[j]->key) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

void	print_export(t_env *env)
{
	t_env	**arr;
	int		i;

	arr = env_to_array(env);
	if (!arr)
		return ;
	sort_env_array(arr);
	i = 0;
	while (arr[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(arr[i]->key, STDOUT_FILENO);
		if (arr[i]->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(arr[i]->value, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
		i++;
	}
	free(arr);
}
