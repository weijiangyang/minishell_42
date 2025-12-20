/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_api.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:34:25 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/20 17:04:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

/*
** 函数作用：对 str 做 $ 展开 + 去引号，并 free 掉旧 str。
** 参数：minishell(提供 envp 和 $? 等), str(会被 free)
** 返回：新分配的字符串，失败返回 NULL
*/
char	*expander_str(t_minishell *minishell, char *str)
{
	char	*tmp;
	char	*clean;
	int		had_q;
	int		q_s;
	int		q_d;

	tmp = NULL;
	clean = NULL;
	if (!str)
		return (NULL);
	tmp = expand_all(minishell, str);
	if (!tmp)
		return (free(str), NULL);
	clean = remove_quotes_flag(tmp, &had_q, &q_s, &q_d);
	if (!clean)
		clean = ft_strdup(tmp);
	free(tmp);
	if (!clean)
		return (free(str), NULL);
	free(str);
	return (clean);
}
