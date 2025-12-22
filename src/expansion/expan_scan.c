/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_scan.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:32:58 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:39:38 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_scan.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   Updated: 2025/12/21 18:30:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"
/*
** 函数作用：把 b 拼到 a 后面，并 free(a)。
** 参数：a(会被 free), b(只读)
** 返回：新字符串；失败返回 NULL
*/
char	*str_join_free(char *a, const char *b)
{
	char	*res;

	res = NULL;
	if (!a && !b)
		return (NULL);
	if (!a)
		return (ft_strdup(b));
	if (!b)
		return (a);
	res = ft_strjoin(a, b);
	free(a);
	return (res);
}

/*
** 函数作用：处理特殊变量：$? 和 $数字。
** 参数：data(上下文), s(源串), j(当前位置)
** 返回：消费的字符数（0 表示没处理）
*/
static int	handle_special_exp(t_exp_data *data, const char *s, int j)
{
	char	*tmp;
	int		status;

	tmp = NULL;
	status = 0;
	if (s[j + 1] == '?')
	{
		if (data->minishell)
			status = data->minishell->last_exit_status;
		tmp = ft_itoa(status);
		if (!tmp)
			return (2);
		*data->out = str_join_free(*data->out, tmp);
		free(tmp);
		return (2);
	}
	if (ft_isdigit((unsigned char)s[j + 1]))
		return (2);
	return (0);
}

/*
** 函数作用：处理普通变量 $VAR。
** 参数：data(上下文), s(源串), j(当前位置)
** 返回：消费的字符数
*/
static int	handle_var_exp(t_exp_data *data, const char *s, int j)
{
	int		len;
	char	*tmp;

	len = 0;
	tmp = NULL;
	len = var_len(&s[j + 1]);
	if (len > 0)
	{
		tmp = env_value_dup(data->minishell, &s[j + 1], len);
		*data->out = str_join_free(*data->out, tmp);
		free(tmp);
		return (1 + len);
	}
	*data->out = str_join_free(*data->out, "$");
	return (1);
}

/*
** 函数作用：展开一次从 s[j] 开始的 $...（考虑引号状态）。
** 参数：data(上下文), s(源串), j(当前位置), q(引号状态)
** 返回：消费的字符数
*/
int	scan_expand_one(t_exp_data *data, const char *s, int j, enum qstate q)
{
	int	res;

	res = 0;
	if (q == Q_SQ)
	{
		*data->out = str_join_free(*data->out, "$");
		return (1);
	}
	res = handle_special_exp(data, s, j);
	if (res > 0)
		return (res);
	res = handle_var_exp(data, s, j);
	return (res);
}
