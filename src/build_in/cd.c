/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:23:39 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/13 15:32:57 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h"

static char	*get_env_value(t_minishell *ms, const char *key)
{
	t_env	*cur;

	if (!ms || !key)
		return (NULL);
	cur = ms->env;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}

static char	*cd_get_target(char **argv, t_minishell *ms)
{
	char	*target;

	if (!argv[1])
	{
		target = get_env_value(ms, "HOME");
		if (!target)
		{
			ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
			return (NULL);
		}
	}
	else if (argv[2])
	{
		ms_put3("cd: ", "too many arguments", "\n");
		return (NULL);
	}
	else
		target = argv[1];
	return (target);
}

static int	cd_get_oldcwd(char *cwd, size_t size)
{
	if (!getcwd(cwd, size))
	{
		perror("getcwd");
		return (-1);
	}
	return (0);
}

static int	cd_change_dir(const char *target)
{
	if (chdir(target) != 0)
	{
		ms_put3("minishell: cd: ", (char *)target,
			": No such file or directory\n");
		return (-1);
	}
	return (0);
}

int	ft_cd(char **argv, t_env **env, t_minishell *msh)
{
	char	cwd[4096];
	char	*target;

	target = cd_get_target(argv, msh);
	if (!target)
		return (1);
	if (cd_get_oldcwd(cwd, sizeof(cwd)) != 0)
		return (1);
	if (cd_change_dir(target) != 0)
		return (1);
	env_set(env, "OLDPWD", cwd);
	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("getcwd");
		return (1);
	}
	env_set(env, "PWD", cwd);
	return (0);
}
