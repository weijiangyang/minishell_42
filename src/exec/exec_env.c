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

/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 10:40:00 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:00:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/exec.h"
#include "../../libft/libft.h"

/*
** 函数作用：
** 从 msh->envp 里找到 PATH=... 并复制出 PATH 的值（新字符串，需要 free）。
** 如果 envp 里没有 PATH，返回 NULL。
*/
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
		i = i + 1;
	}
	return (NULL);
}

/*
** 函数作用：
** 释放 msh->paths（PATH 切出来的目录数组），防止内存泄漏。
*/
void	free_paths(t_minishell *msh)
{
	int	i;

	i = 0;
	if (!msh || !msh->paths)
		return ;
	while (msh->paths[i])
	{
		free(msh->paths[i]);
		i = i + 1;
	}
	free(msh->paths);
	msh->paths = NULL;
}

/*
** 函数作用：
** 重新读取 envp 里的 PATH，并更新 msh->paths。
** 这样 export PATH=... 之后，找命令路径才会用新的 PATH。
** 返回值：
** - 成功返回 0
** - 内存分配失败返回 1
*/
int	exec_refresh_paths(t_minishell *msh)
{
	char	*raw;

	raw = NULL;
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

/*
** 函数作用：
** 确保 msh->paths 已经准备好：第一次用才生成。
** 返回值：
** - 已有 paths 或刷新成功：返回 0
** - 刷新失败：返回 1
*/
int	ensure_paths_ready(t_minishell *msh)
{
	if (!msh)
		return (1);
	if (msh->paths)
		return (0);
	return (exec_refresh_paths(msh));
}
