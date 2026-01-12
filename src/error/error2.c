/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 17:22:21 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 15:13:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

void	ms_perror(const char *msg)
{
	ms_put3("minishell: ", NULL, NULL);
	perror(msg);
}

void	ms_err_syntax_unexpected(const char *tok)
{
	ms_put3("minishell: syntax error near unexpected token `", tok, "'\n");
}

void	ms_err_eof_quote(char q)
{
	char	str[2];

	str[0] = q;
	str[1] = '\0';
	ms_put3("minishell: unexpected EOF looking for `", str, "'\n");
	ms_put3("minishell: syntax error: unexpected end of file\n", NULL, NULL);
}
