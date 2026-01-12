/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_nosubshell_cmd_redir_list.c                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:53:10 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:53:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

static int	process_tokens(t_lexer **cur, t_redir **redir,
				t_cmd **argv_cmd, t_minishell *msh)
{
	t_lexer	*pt;

	pt = peek_token(cur);
	while (pt != NULL)
	{
		if (is_redir_token(pt))
		{
			if (!build_redir(cur, redir, msh))
				return (0);
		}
		else if (pt->tokentype == TOK_WORD)
			ft_lstadd_back(argv_cmd, create_argv(consume_token(cur)->raw));
		else
			break ;
		pt = peek_token(cur);
	}
	return (1);
}

static t_ast	*finalize_node(t_ast *node, t_cmd *argv_cmd,
					t_redir *redir, t_minishell *msh)
{
	if (!argv_cmd && !redir)
	{
		if (msh->last_exit_status != 130)
			msh->last_exit_status = 2;
		return (NULL);
	}
	node->type = NODE_CMD;
	node->redir = redir;
	node->argv = build_argvs(argv_cmd, redir, node);
	return (node);
}

t_ast	*parse_normal_cmd_redir_list(t_lexer **cur, t_ast *node,
		t_minishell *ms)
{
	t_redir	*redir;
	t_cmd	*argv_cmd;

	redir = NULL;
	argv_cmd = NULL;
	if (!cur || !node)
		return (NULL);
	if (!process_tokens(cur, &redir, &argv_cmd, ms))
	{
		free_redir_list(redir);
		free_argv_list(argv_cmd);
		return (NULL);
	}
	return (finalize_node(node, argv_cmd, redir, ms));
}
