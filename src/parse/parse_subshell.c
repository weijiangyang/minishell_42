/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_subshell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:30:13 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:30:14 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

t_ast	*parse_subshell(t_lexer **cur, t_ast *node, t_minishell *minishell)
{
	consume_token(cur);
	node->type = NODE_SUBSHELL;
	node->sub = parse_pipeline(cur, minishell);
	if (!node->sub)
	{
		free(node);
		return (NULL);
	}
	if (!expect_token(TOK_RPAREN, cur))
	{
		ft_putstr_fd("minishell: syntax error: expected `)'\n", 2);
		minishell->last_exit_status = 2;
		free_ast(node);
		return (NULL);
	}
	return (node);
}
