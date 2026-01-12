/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_simple_cmd_redir_list.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:10 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:19:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

t_ast	*parse_simple_cmd_redir_list(t_lexer **cur, t_minishell *minishell)
{
	t_ast	*node;
	t_lexer	*pt;

	pt = peek_token(cur);
	node = ft_calloc(1, sizeof(t_ast));
	if (!node)
		return (NULL);
	if (pt && pt->tokentype == TOK_LPAREN)
		return (parse_subshell(cur, node, minishell));
	return (parse_normal_cmd_redir_list(cur, node, minishell));
}
