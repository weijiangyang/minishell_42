/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 22:41:04 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:41:09 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"
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
