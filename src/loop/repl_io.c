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

static char	*ms_read_line_raw(int fd)
{
	char	*buf;
	size_t	size;
	size_t	len;
	char	ch;

	size = 64;
	len = 0;
	ch = '\0';
	buf = (char *)malloc(size);
	if (!buf)
		return (NULL);
	while (read(fd, &ch, 1) > 0 && ch != '\n')
	{
		if (len + 1 >= size)
		{
			buf = ms_expand_buffer(buf, &size, len);
			if (!buf)
				return (NULL);
		}
		buf[len++] = ch;
	}
	if (len == 0 && ch != '\n')
		return (free(buf), NULL);
	buf[len] = '\0';
	return (buf);
}

static char	*ms_obtain_line(const char *prompt)
{
	if (isatty(STDIN_FILENO))
		return (readline(prompt));
	return (ms_read_line_raw(STDIN_FILENO));
}

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
