/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_argvs.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:52:27 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:52:30 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

t_cmd	*create_argv(char *str)
{
	char	*dup;

	if (!str)
		return (NULL);
	dup = ft_strdup(str);
	if (!dup)
		return (NULL);
	return (ft_lstnew(dup));
}

static void	cleanup_build_fail(t_cmd *argv_cmd, t_redir *redir, t_ast *node)
{
	free_redir_list(redir);
	free_argv_list(argv_cmd);
	free(node);
}

char	**build_argvs(t_cmd *argv_cmd, t_redir *redir, t_ast *node)
{
	int		size;
	char	**argvs;
	int		i;
	t_cmd	*tmp;

	if (argv_cmd == NULL)
		return (NULL);
	size = ft_lstsize(argv_cmd);
	argvs = malloc((size + 1) * sizeof(char *));
	if (!argvs)
	{
		cleanup_build_fail(argv_cmd, redir, node);
		return (NULL);
	}
	i = 0;
	tmp = argv_cmd;
	while (tmp && i < size)
	{
		argvs[i++] = tmp->content;
		tmp = tmp->next;
	}
	argvs[i] = NULL;
	free_t_cmd_node(argv_cmd);
	return (argvs);
}
