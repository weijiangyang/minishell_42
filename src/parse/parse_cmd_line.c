/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                        +#+  +:+       +#+    */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 22:19:34 by yang              #+#    #+#             */
/*   Updated: 2025/10/31 22:19:38 by yang             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

t_ast	*parse_cmdline(t_lexer **cur, t_minishell *minishell)
{
	t_ast	*root;
	t_lexer	*pt;

	root = parse_pipeline(cur, minishell);
	if (!root)
		return (NULL);
	pt = peek_token(cur);
	if (pt && pt->tokentype != TOK_END)
	{
		ms_put3("minishell: syntax error: unexpected token `",
			pt->raw, "'\n");
		free_ast(root);
		return (NULL);
	}
	return (root);
}
