/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 19:39:04 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 02:57:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

/* 挪到这里：判断是否有引号 */
int	word_has_quotes(const char *s)
{
	int	i;

	i = 0;
	while (s && s[i])
	{
		if (s[i] == '\'' || s[i] == '"')
			return (1);
		i++;
	}
	return (0);
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
** 函数作用：去掉字符串 s 里的引号；如果没有引号，就返回 s 的副本。
** 参数：s(输入字符串), had_q/q_s/q_d(输出：有没有引号、单引号、双引号)
** 返回：新字符串；失败返回 NULL
*/
char	*strip_all_quotes_dup(const char *s, int *had_q, int *q_s, int *q_d)
{
	char	*clean;

	clean = NULL;
	if (!s)
		return (NULL);
	clean = remove_quotes_flag(s, had_q, q_s, q_d);
	if (!clean)
		clean = ft_strdup(s);
	return (clean);
}
