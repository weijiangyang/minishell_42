/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_io.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:36 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/29 18:10:57 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"


/* 函数作用：非交互模式读一行，只读到 '\n' 就停，避免吞掉后续 bash 命令 */
static char	*ms_read_line_raw(int fd)
{
	char	*buf;
	char	*tmp;
	size_t	st[2];
	ssize_t	rd;
	char	ch;

	buf = NULL;
	tmp = NULL;
	st[0] = 64;
	st[1] = 0;
	buf = (char *)malloc(st[0]);
	if (!buf)
		return (NULL);
	rd = 1;
	while (rd > 0)
	{
		rd = read(fd, &ch, 1);
		if (rd <= 0 || ch == '\n')
			break ;
		if (st[1] + 1 >= st[0])
		{
			st[0] = st[0] * 2;
			tmp = (char *)malloc(st[0]);
			if (!tmp)
				return (free(buf), NULL);
			ft_memcpy(tmp, buf, st[1]);
			free(buf);
			buf = tmp;
		}
		buf[st[1]] = ch;
		st[1] = st[1] + 1;
	}
	if (rd <= 0 && st[1] == 0)
		return (free(buf), NULL);
	buf[st[1]] = '\0';
	return (buf);
}

static char	*ms_read_line(const char *prompt)
{
	char	*line;

	line = NULL;
	if (isatty(STDIN_FILENO))
		line = readline(prompt);
	else
		line = ms_read_line_raw(STDIN_FILENO);
	return (line);
}

/*
** 函数作用：
**   读取用户一行输入。
** 提示符规则：
**   - acc 有内容：说明在续行 → "> "
**   - acc 为空：主提示符 → "minishell$ "
** 非交互模式（stdin 不是 tty）：
**   - 不打印提示符（prompt 传空串）。
*/
char	*repl_read(char *acc)
{
	char	*prompt;

	prompt = "";
	if (isatty(STDIN_FILENO))
	{
		if (acc && acc[0] != '\0')
			prompt = "> ";
		else
			prompt = "minishell$ ";
	}
	return (ms_read_line(prompt));
}

/*
** 函数作用：
**   判断字符串里是否有非空白字符。
** 说明：
**   用户只输入空格/Tab/回车时，不应该当作一条命令。
*/
int	repl_has_text(const char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (!check_space((unsigned char)s[i]))
			return (1);
		i++;
	}
	return (0);
}

/*
** 函数作用：
**   把 line 拼到 acc 后面。
** 规则：
**   - acc 为空：acc = strdup(line)
**   - acc 非空：acc = acc + "\n" + line
*/
int	repl_join(char **acc, char *line)
{
	char	*tmp;

	if (!acc || !line)
		return (0);
	if (!*acc)
	{
		*acc = ft_strdup(line);
		return (*acc != NULL);
	}
	tmp = ft_strjoin(*acc, "\n");
	free(*acc);
	*acc = tmp;
	if (!*acc)
		return (0);
	tmp = ft_strjoin(*acc, line);
	free(*acc);
	*acc = tmp;
	return (*acc != NULL);
}
