/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 22:33:48 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:35:52 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "repl.h"

int	lexer_has_heredoc(t_lexer *lx)
{
	while (lx)
	{
		if (lx->tokentype == TOK_HEREDOC)
			return (1);
		lx = lx->next;
	}
	return (0);
}

char	*dup_first_line(const char *s)
{
	size_t	i;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	return (ft_substr(s, 0, i));
}

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

void	repl_free_acc(char **acc)
{
	if (!acc)
		return ;
	if (*acc)
		free(*acc);
	*acc = NULL;
}
