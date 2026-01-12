/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_redir_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:18:45 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:18:51 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

static t_redir_type	get_redir_type(t_tok_type type)
{
	if (type == TOK_REDIR_IN)
		return (REDIR_INPUT);
	if (type == TOK_REDIR_OUT)
		return (REDIR_OUTPUT);
	if (type == TOK_APPEND)
		return (REDIR_APPEND);
	return (HEREDOC);
}

static t_redir	*create_redir(t_tok_type type, char *content)
{
	t_redir	*new_node;

	new_node = ft_calloc(1, sizeof(t_redir));
	if (!new_node)
		return (NULL);
	if (content)
		new_node->filename = ft_strdup(content);
	if (!content || !new_node->filename)
	{
		free(new_node);
		return (NULL);
	}
	new_node->heredoc_fd = -1;
	new_node->type = get_redir_type(type);
	return (new_node);
}

static void	redirlst_add_back(t_redir **lst, t_redir *new_node)
{
	t_redir	*tmp;

	if (!lst || !new_node)
		return ;
	if (!*lst)
	{
		*lst = new_node;
		return ;
	}
	tmp = *lst;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

static int	consume_redir_pair(t_lexer **cur, t_lexer **op, t_lexer **filetok,
								t_minishell *ms)
{
	*op = consume_token(cur);
	if (!*op)
		return (0);
	*filetok = consume_token(cur);
	if (!*filetok || (*filetok)->tokentype != TOK_WORD)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
		if (ms)
			ms->last_exit_status = 2;
		return (0);
	}
	return (1);
}

int	build_redir(t_lexer **cur, t_redir **redir_list, t_minishell *ms)
{
	t_lexer	*op;
	t_lexer	*filetok;
	t_redir	*new_redir;
	char	*text;

	if (!consume_redir_pair(cur, &op, &filetok, ms))
		return (0);
	text = filetok->raw;
	if (op->tokentype == TOK_HEREDOC)
		text = filetok->str;
	new_redir = create_redir(op->tokentype, text);
	if (!new_redir)
		return (0);
	if (op->tokentype == TOK_HEREDOC)
		new_redir->quoted = filetok->had_quotes;
	redirlst_add_back(redir_list, new_redir);
	return (1);
}
