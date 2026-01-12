/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:15:42 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/09 00:56:19 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"
#include "parse.h"

static char	*join_full_line(char *full_line, char *line)
{
	char	*tmp;

	if (!full_line)
		return (line);
	tmp = ft_strjoin(full_line, line);
	free(full_line);
	free(line);
	return (tmp);
}

static void	handle_heredoc_exp(char **line, t_minishell *msh, int quoted)
{
	char	*old;
	char	*expanded;

	if (!quoted)
	{
		old = *line;
		expanded = expand_heredoc_vars(msh, old);
		if (expanded != NULL)
		{
			*line = expanded;
			free(old);
		}
	}
}

static int	process_heredoc_line(char **line, char **full_line,
		const char *delimiter, t_minishell *msh)
{
	size_t	len;

	len = ft_strlen(*line);
	if (len > 0 && (*line)[len - 1] == '\n')
	{
		(*line)[len - 1] = '\0';
		if (ft_strcmp(*line, delimiter) == 0)
		{
			free(*line);
			*full_line = NULL;
			return (1);
		}
		handle_heredoc_exp(line, msh, msh->heredoc_quoted);
		write(msh->heredoc_write_fd, *line, ft_strlen(*line));
		write(msh->heredoc_write_fd, "\n", 1);
		free(*line);
		*full_line = NULL;
		return (0);
	}
	*full_line = *line;
	return (0);
}

static int	read_and_process_line(char **full_line, const char *delimiter,
		t_minishell *msh)
{
	char	*line;

	line = get_next_line(STDIN_FILENO);
	if (g_signal == SIGINT)
	{
		free(line);
		free(*full_line);
		write(1, "\n", 1);
		return (-1);
	}
	if (!line && *full_line == NULL)
		return (1);
	if (!line)
	{
		line = *full_line;
		*full_line = NULL;
	}
	else if (*full_line)
		line = join_full_line(*full_line, line);
	if (process_heredoc_line(&line, full_line, delimiter, msh))
		return (2);
	return (0);
}

int	heredoc_loop(int write_fd, const char *delimiter, t_minishell *msh,
		int quoted)
{
	char	*full_line;
	int		status;

	full_line = NULL;
	msh->heredoc_write_fd = write_fd;
	msh->heredoc_quoted = quoted;
	setup_heredoc_signals();
	while (1)
	{
		if (!full_line)
			write(STDOUT_FILENO, "heredoc> ", 9);
		g_signal = 0;
		status = read_and_process_line(&full_line, delimiter, msh);
		if (status != 0)
		{
			if (status == -1)
				return (-1);
			return (0);
		}
	}
}
