/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_io.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:36 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/28 02:37:02 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "repl.h"



/* 作用：判断当前是否交互模式（stdin/stdout 都是 tty）。 */
static int	repl_is_interactive(void)
{
	int	ok;

	ok = 0;
	if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
		ok = 1;
	return (ok);
}

/*
** 函数作用：
**   读一行输入：
**   - 交互模式：readline + prompt
**   - 非交互模式（管道/文件喂给 stdin）：get_next_line，不输出 prompt
** 参数：
**   acc：已累计的输入（NULL 表示第一行）
** 返回：
**   堆字符串；EOF 时返回 NULL
*/
char	*repl_read(char *acc)
{
	char	*line;
	char	*prompt;
	size_t	len;

	line = NULL;
	prompt = "minishell$ ";
	len = 0;
	if (repl_is_interactive())
	{
		if (acc)
			prompt = "> ";
		line = readline(prompt);
		return (line);
	}
	line = get_next_line(STDIN_FILENO);
	if (!line)
		return (NULL);
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	return (line);
}

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
