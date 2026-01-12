/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_scan.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:32:58 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

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

int	scan_expand_one(t_exp_data *data, const char *s, int j, enum e_qstate q)
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
