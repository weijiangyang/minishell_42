/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_io.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:36 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 19:27:06 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

/* 作用：当存储空间不够时，把盒子扩大一倍。 */
static char	*ms_expand_buffer(char *buf, size_t *size, size_t len)
{
	char	*new_buf;

	*size = (*size) * 2;
	new_buf = (char *)malloc(*size);
	if (!new_buf)
	{
		free(buf);
		return (NULL);
	}
	ft_memcpy(new_buf, buf, len);
	free(buf);
	return (new_buf);
}

/* 作用：在非交互模式下，一个字符一个字符地读取一行。 */
static char	*ms_read_line_raw(int fd)
{
	char	*buf;
	size_t	st[2];
	char	ch;

	st[0] = 64;
	st[1] = 0;
	buf = (char *)malloc(st[0]);
	if (!buf)
		return (NULL);
	while (read(fd, &ch, 1) > 0 && ch != '\n')
	{
		if (st[1] + 1 >= st[0])
		{
			buf = ms_expand_buffer(buf, &st[0], st[1]);
			if (!buf)
				return (NULL);
		}
		buf[st[1]++] = ch;
	}
	if (st[1] == 0 && ch != '\n')
		return (free(buf), NULL);
	buf[st[1]] = '\0';
	return (buf);
}

/* 作用：根据当前环境选择最合适的读取方式。 */
static char	*ms_obtain_line(const char *prompt)
{
	if (isatty(STDIN_FILENO))
		return (readline(prompt));
	return (ms_read_line_raw(STDIN_FILENO));
}

/* 作用：根据命令是否写完，显示不同的提示符并读取输入。 */
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
	return (ms_obtain_line(prompt));
}

/* 作用：检查这一行是不是只写了空格或 Tab。 */
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

