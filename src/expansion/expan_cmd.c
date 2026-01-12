/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 21:03:52 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 18:20:12 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

static int	strip_quotes_inplace(char **s)
{
	char	*tmp;
	int		had_q;
	int		q_s;
	int		q_d;

	had_q = 0;
	q_s = 0;
	q_d = 0;
	if (!s || !*s)
		return (1);
	tmp = remove_quotes_flag(*s, &had_q, &q_s, &q_d);
	if (!tmp)
		tmp = ft_strdup(*s);
	if (!tmp)
		return (0);
	free(*s);
	*s = tmp;
	return (1);
}

static void	argv_shift_left(char **argv, int i)
{
	while (argv[i])
	{
		argv[i] = argv[i + 1];
		i++;
	}
}

static int	expand_argv_inplace(t_minishell *msh, t_ast *node)
{
	int		i;
	int		had_q;
	char	*tmp;

	i = 0;
	while (node->argv && node->argv[i])
	{
		had_q = word_has_quotes(node->argv[i]);
		tmp = expander_str(msh, node->argv[i]);
		if (!tmp)
			return (0);
		if (tmp[0] == '\0' && had_q == 0)
		{
			free(tmp);
			argv_shift_left(node->argv, i);
		}
		else
		{
			node->argv[i] = tmp;
			i++;
		}
	}
	return (1);
}

static int	expand_redirs_inplace(t_minishell *msh, t_redir *r)
{
	char	*tmp;

	while (r)
	{
		if (r->filename && r->type == HEREDOC)
		{
			if (!strip_quotes_inplace(&r->filename))
				return (0);
		}
		else if (r->filename)
		{
			tmp = expander_str(msh, r->filename);
			if (!tmp)
				return (0);
			r->filename = tmp;
		}
		r = r->next;
	}
	return (1);
}

int	expander_expand_cmd_node(t_minishell *msh, t_ast *node)
{
	if (!msh || !node || node->type != NODE_CMD)
		return (1);
	if (!expand_argv_inplace(msh, node))
		return (0);
	if (!expand_redirs_inplace(msh, node->redir))
		return (0);
	return (1);
}
