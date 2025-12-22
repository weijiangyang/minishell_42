/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:16:05 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:21:18 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/exec.h"
#include "../../libft/libft.h"

static char	*join_dir_cmd(const char *dir, const char *cmd)
{
	char	*tmp;
	char	*res;

	tmp = NULL;
	res = NULL;
	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	res = ft_strjoin(tmp, cmd);
	free(tmp);
	return (res);
}

/*
** 函数作用：
** 用 PATH 找到可执行文件的完整路径（返回新字符串，需要 free）。
** 如果命令本来就带 '/'，就直接当作路径复制返回。
** 关键修复：
** - 如果在 PATH 里遇到“存在但不可执行”的同名文件，也直接返回该路径，
**   让 execve 产生 EACCES/EISDIR，并映射到 126（更贴近 bash）。
*/
char	*find_cmd_path(t_minishell *msh, const char *cmd)
{
	int		i;
	char	*path;

	i = 0;
	path = NULL;
	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	ensure_paths_ready(msh);
	while (msh->paths && msh->paths[i])
	{
		path = join_dir_cmd(msh->paths[i], cmd);
		if (!path)
			return (NULL);
		if (access(path, F_OK) == 0)
			return (path);
		free(path);
		i = i + 1;
	}
	return (NULL);
}
