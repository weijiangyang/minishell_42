/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:31:17 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:31:19 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"
#include "error.h"

t_lexer	*peek_token(t_lexer **cur)
{
	if (!cur)
		return (NULL);
	return (*cur);
}

t_lexer	*consume_token(t_lexer **cur)
{
	t_lexer	*old;

	if (!cur || !*cur)
		return (NULL);
	old = *cur;
	*cur = (*cur)->next;
	return (old);
}

t_lexer	*expect_token(t_tok_type type, t_lexer **cur)
{
	if (!cur || !*cur || (*cur)->tokentype != type)
	{
		if (cur && *cur)
			ms_put3("minishell: syntax error near unexpected token `",
				(char *)t_tok_type_to_str((*cur)->tokentype), "'\n");
		else
			ms_put3("minishell: syntax error: unexpected end of file\n",
				NULL, NULL);
		return (NULL);
	}
	return (consume_token(cur));
}

int	is_redir_token(t_lexer *pt)
{
	if (pt->tokentype == TOK_REDIR_IN
		|| pt->tokentype == TOK_REDIR_OUT
		|| pt->tokentype == TOK_APPEND
		|| pt->tokentype == TOK_HEREDOC)
		return (1);
	return (0);
}

const char	*t_tok_type_to_str(t_tok_type type)
{
	if (type == TOK_WORD)
		return ("WORD");
	if (type == TOK_PIPE)
		return ("|");
	if (type == TOK_REDIR_IN)
		return ("<");
	if (type == TOK_REDIR_OUT)
		return (">");
	if (type == TOK_APPEND)
		return (">>");
	if (type == TOK_HEREDOC)
		return ("<<");
	if (type == TOK_END)
		return ("newline");
	if (type == TOK_ERROR)
		return ("ERROR");
	return ("UNKNOWN");
}
