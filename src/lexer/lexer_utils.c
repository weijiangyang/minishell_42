/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:30:24 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 18:46:30 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

const char	*token_symbol(t_tok_type t)
{
	if (t == TOK_PIPE)
		return ("|");
	if (t == TOK_REDIR_OUT)
		return (">");
	if (t == TOK_REDIR_IN)
		return ("<");
	if (t == TOK_APPEND)
		return (">>");
	if (t == TOK_HEREDOC)
		return ("<<");
	return (NULL);
}

int	detect_signal(t_lexer **list)
{
	if (g_signal == SIGINT)
	{
		clear_list(list);
		g_signal = 0;
		return (1);
	}
	return (0);
}

static void	release_word_memory(t_token_info *info)
{
	if (info->raw && info->clean && info->raw != info->clean)
	{
		free(info->raw);
		free(info->clean);
	}
	else if (info->raw)
		free(info->raw);
	else if (info->clean)
		free(info->clean);
}

int	finalize_word_node(t_token_info *info, t_lexer **list)
{
	if (!add_node(info, TOK_WORD, list))
	{
		release_word_memory(info);
		return (-1);
	}
	return (1);
}
