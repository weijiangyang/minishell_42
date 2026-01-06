/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 19:39:04 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 19:50:11 by yzhang2          ###   ########.fr       */
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
