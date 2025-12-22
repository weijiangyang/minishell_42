/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_scan2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:32:30 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:40:33 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"

/*
** 函数作用：遇到 ' 或 " 时切换引号状态（单引号里不进入双引号，反过来也一样）。
** 参数：c(当前字符), q(引号状态指针)
*/
static void	toggle_quote_state(const char c, enum qstate *q)
{
	if (c == '\'' && *q != Q_DQ)
		*q = (*q == Q_SQ) ? Q_NONE : Q_SQ;
	else if (c == '\"' && *q != Q_SQ)
		*q = (*q == Q_DQ) ? Q_NONE : Q_DQ;
}

/*
** 函数作用：把一个普通字符追加到输出字符串里。
** 参数：c(字符), out(输出字符串指针)
** 返回：固定 1（表示消费 1 个字符）
*/
static int	append_char(const char c, char **out)
{
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	*out = str_join_free(*out, buf);
	return (1);
}

/*
** 函数作用：整串做 $ 展开（保留引号字符本身，不在这里去引号）。
** 参数：minishell(全局上下文), str(输入字符串)
** 返回：新字符串；失败返回 NULL
*/
char	*expand_all(t_minishell *minishell, const char *str)
{
	int			i;
	char		*out;
	enum qstate	q;
	t_exp_data	data;

	i = 0;
	out = NULL;
	q = Q_NONE;
	if (!str)
		return (NULL);
	out = ft_strdup("");
	if (!out)
		return (NULL);
	data.minishell = minishell;
	data.out = &out;
	while (str[i])
	{
		toggle_quote_state(str[i], &q);
		if (str[i] == '$')
			i = i + scan_expand_one(&data, str, i, q);
		else
			i = i + append_char(str[i], &out);
	}
	return (out);
}
