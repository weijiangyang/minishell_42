/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 16:21:54 by yzhang2           #+#    #+#             */
/*   Updated: 2025/08/12 15:39:55 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*join_free(char *s1, char *s2)
{
	char	*joined;
	char	*tmp;

	tmp = s1;
	joined = gnl_strjoin(tmp, s2);
	free(tmp);
	return (joined);
}

char	*read_save(int fd, char *backup)
{
	ssize_t	count;
	char	*buf;

	if (!backup)
		backup = gnl_strdup("");
	if (!backup)
		return (NULL);
	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (free(backup), NULL);
	count = 1;
	while (backup && !gnl_strchr(backup, '\n') && count > 0)
	{
		count = read(fd, buf, BUFFER_SIZE);
		if (count < 0)
			return (free(buf), free(backup), NULL);
		if (count == 0)
			break ;
		buf[count] = '\0';
		backup = join_free(backup, buf);
		if (!backup)
			return (free(buf), NULL);
	}
	return (free(buf), backup);
}

char	*get_the_line(char *backup)
{
	char	*line;
	size_t	i;

	i = 0;
	if (!backup || !*backup)
		return (NULL);
	while (backup[i] && backup[i] != '\n')
		i++;
	if (backup[i] == '\n')
		i++;
	line = gnl_substr(backup, 0, i);
	return (line);
}

char	*refresh_backup(char *backup)
{
	size_t	i;
	char	*fresh;

	if (!backup)
		return (NULL);
	i = 0;
	while (backup[i] && backup[i] != '\n')
		i++;
	if (!backup[i])
	{
		free(backup);
		return (NULL);
	}
	fresh = gnl_strdup(backup + i + 1);
	free(backup);
	return (fresh);
}

char	*get_next_line(int fd)
{
	static char	*backup;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	backup = read_save(fd, backup);
	if (!backup)
		return (NULL);
	line = get_the_line(backup);
	backup = refresh_backup(backup);
	if (!line)
	{
		free(backup);
		backup = NULL;
		return (NULL);
	}
	return (line);
}
