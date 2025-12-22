/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_io.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:36 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/22 17:29:40 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "repl.h"

/*
** 函数作用：
**   读一行输入：如果 acc 为空用主提示符，否则用续行提示符。
** 参数：
**   acc：已累计的输入（NULL 表示第一行）
** 返回：
**   readline 返回的堆字符串；Ctrl-D 时返回 NULL
*/
char	*repl_read(char *acc)
{
	char	*line;
	char	*prompt;

	line = NULL;
	prompt = "minishell$ ";
	if (acc)
		prompt = "> ";
	line = readline(prompt);
	return (line);
}

/*
** 函数作用：
**   把 line 拼接进 acc：
**   - acc 为 NULL：acc = strdup(line)
**   - acc 非 NULL：acc = acc + "\n" + line
** 参数：
**   acc：指向累计字符串的指针
**   line：新读入的一行（函数内部会 free(line)）
** 返回：
**   1 成功，0 失败（malloc 失败）
*/
int	repl_join(char **acc, char *line)
{
	char	*tmp;
	size_t	a;
	size_t	b;

	tmp = NULL;
	a = 0;
	b = 0;
	if (!acc || !line)
		return (0);
	if (*acc == NULL)
	{
		tmp = ft_strdup(line);
		free(line);
		if (!tmp)
			return (0);
		*acc = tmp;
		return (1);
	}
	a = ft_strlen(*acc);
	b = ft_strlen(line);
	tmp = malloc(a + 1 + b + 1);
	if (!tmp)
		return (free(line), 0);
	ft_memcpy(tmp, *acc, a);
	tmp[a] = '\n';
	ft_memcpy(tmp + a + 1, line, b);
	tmp[a + 1 + b] = '\0';
	free(*acc);
	free(line);
	*acc = tmp;
	return (1);
}

/*
** 函数作用：
**   判断 s 是否含有非空白字符（用于决定是否 add_history）。
** 参数：
**   s：字符串
** 返回：
**   1 表示有内容，0 表示全是空白或为空
*/
int	repl_has_text(const char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
	{
		if (check_space(s[i]) == 0)
			return (1);
		i = i + 1;
	}
	return (0);
}
