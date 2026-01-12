/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 10:40:00 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:22:22 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "exec.h"

static char	*dup_env_path(t_minishell *msh)
{
	int		i;
	char	*line;

	i = 0;
	line = NULL;
	if (!msh || !msh->envp)
		return (NULL);
	while (msh->envp[i])
	{
		line = msh->envp[i];
		if (ft_strncmp(line, "PATH=", 5) == 0)
			return (ft_strdup(line + 5));
		i++;
	}
	return (NULL);
}

void	free_paths(t_minishell *msh)
{
	int	i;

	if (!msh || !msh->paths)
		return ;
	i = 0;
	while (msh->paths[i])
	{
		free(msh->paths[i]);
		i++;
	}
	free(msh->paths);
	msh->paths = NULL;
}

int	exec_refresh_paths(t_minishell *msh)
{
	char	*raw;

	if (!msh)
		return (1);
	free_paths(msh);
	raw = dup_env_path(msh);
	if (!raw)
		return (0);
	msh->paths = ft_split(raw, ':');
	free(raw);
	if (!msh->paths)
		return (1);
	return (0);
}

int	ensure_paths_ready(t_minishell *msh)
{
	if (!msh)
		return (1);
	if (msh->paths)
		return (0);
	return (exec_refresh_paths(msh));
}
