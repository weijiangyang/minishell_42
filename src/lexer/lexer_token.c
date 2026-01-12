/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 04:44:56 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:24:02 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

t_tok_type	is_token(int c)
{
	if (c == '|')
		return (TOK_PIPE);
	if (c == '>')
		return (TOK_REDIR_OUT);
	if (c == '<')
		return (TOK_REDIR_IN);
	return (0);
}

static int	handle_double_token(t_tok_type tokentype, int next_char,
								t_lexer **list)
{
	if (tokentype == TOK_REDIR_OUT && is_token(next_char) == TOK_REDIR_OUT)
	{
		if (!add_node(NULL, TOK_APPEND, list))
			return (-1);
		return (2);
	}
	if (tokentype == TOK_REDIR_IN && is_token(next_char) == TOK_REDIR_IN)
	{
		if (!add_node(NULL, TOK_HEREDOC, list))
			return (-1);
		return (2);
	}
	return (0);
}

int	handle_token(char *str, int i, t_lexer **list)
{
	t_tok_type	token;
	int			next_char;
	int			res;

	token = is_token((unsigned char)str[i]);
	next_char = (unsigned char)str[i + 1];
	res = handle_double_token(token, next_char, list);
	if (res != 0)
		return (res);
	if (token != 0)
	{
		if (!add_node(NULL, token, list))
			return (-1);
		return (1);
	}
	return (0);
}

int	match_quotes(int i, char *str, char quote)
{
	int	j;

	j = 0;
	if (str[i + j] == quote)
	{
		j = j + 1;
		while (str[i + j] != quote && str[i + j])
			j = j + 1;
		if (str[i + j] == quote)
			j = j + 1;
		else
			return (-1);
	}
	return (j);
}
