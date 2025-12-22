/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_api.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:34:25 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:36:42 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"
/*
** 函数作用：把 s 去引号；如果本来没有引号，就复制一份 s。
** 参数：s(输入字符串)
** 返回：新分配的字符串，失败返回 NULL
*/
static char	*api_clean_dup(const char *s)
{
	char	*clean;
	int		had_q;
	int		q_s;
	int		q_d;

	clean = NULL;
	had_q = 0;
	q_s = 0;
	q_d = 0;
	clean = remove_quotes_flag(s, &had_q, &q_s, &q_d);
	if (!clean)
		clean = ft_strdup(s);
	return (clean);
}

/*
** 函数作用：对 str 做 $ 展开 + 去引号，并 free 掉旧 str。
** 参数：minishell(提供 envp 和 $? 等), str(会被 free)
** 返回：新分配的字符串，失败返回 NULL
*/
char	*expander_str(t_minishell *minishell, char *str)
{
	char	*tmp;
	char	*clean;

	tmp = NULL;
	clean = NULL;
	if (!str)
		return (NULL);
	tmp = expand_all(minishell, str);
	if (!tmp)
	{
		free(str);
		return (NULL);
	}
	clean = api_clean_dup(tmp);
	free(tmp);
	free(str);
	return (clean);
}
