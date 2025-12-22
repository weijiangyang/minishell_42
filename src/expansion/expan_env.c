/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:34:06 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:38:12 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"
/*
** 函数作用：判断变量名首字符是否合法（字母或 _）。
** 参数：c(一个字符)
** 返回：是 1，不是 0
*/
int	is_name_start(int c)
{
	return (ft_isalpha(c) || c == '_');
}

/*
** 函数作用：判断变量名后续字符是否合法（字母/数字/_）。
** 参数：c(一个字符)
** 返回：是 1，不是 0
*/
int	is_name_char(int c)
{
	return (ft_isalnum(c) || c == '_');
}

/*
** 函数作用：从 s 开始计算合法变量名长度（s 指向 $ 后第一个字符）。
** 参数：s(指向变量名开头)
** 返回：变量名长度（可能为 0）
*/
int	var_len(const char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (0);
	if (!is_name_start((unsigned char)s[i]))
		return (0);
	while (s[i] && is_name_char((unsigned char)s[i]))
		i = i + 1;
	return (i);
}

/*
** 做什么：
**   返回 entry 中第一个 '=' 的索引位置。
**   若没找到 '='，则返回字符串长度（即 '\0' 的位置）。
**
** 举例：
**   "PATH=/usr/bin" → 4
**   "HOME=/home/user" → 4
**   "SHELL" → 5（即 strlen("SHELL")）
**
** 谁调：
**   env_value_dup()（在解析环境变量名时）
*/
size_t	equal_sign(char *entry)
{
	int	i;

	if (!entry)
		return (0);
	i = 0;
	while (entry[i] && entry[i] != '=')
		i++;
	return (i);
}

/*
** 函数作用：在 minishell->envp 里找变量 name[0..len-1]，并复制它的值。
** 参数：minishell(全局上下文), name(变量名起点), len(变量名长度)
** 返回：找到则返回值的 strdup；找不到返回 strdup("")；失败返回 NULL
*/
char	*env_value_dup(t_minishell *minishell, const char *name, int len)
{
	int		k;
	int		keylen;
	char	*entry;

	k = 0;
	keylen = 0;
	entry = NULL;
	if (!minishell || !minishell->envp || !name)
		return (ft_strdup(""));
	while (minishell->envp[k])
	{
		entry = minishell->envp[k];
		keylen = (int)equal_sign(entry);
		if (keylen == len && ft_strncmp(name, entry, len) == 0)
		{
			if (entry[keylen] == '=')
				return (ft_strdup(entry + keylen + 1));
			return (ft_strdup(entry + keylen));
		}
		k = k + 1;
	}
	return (ft_strdup(""));
}
