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


#include "../../include/error.h"
#include "../../include/minishell.h"

/*
** 函数作用：
** 打印系统调用失败的错误（带 minishell 前缀），格式尽量贴近 minishell。
** 例：minishell: line 1: fork: Resource temporarily unavailable
*/
void    ms_perror(const char *msg)
{
    ms_put3("minishell: ", NULL, NULL);
    perror(msg);
}


void    ms_err_syntax_unexpected(const char *tok)
{
    ms_put3("minishell: syntax error near unexpected token `", NULL, NULL);
    ms_put3(tok, "'\n", NULL);
}

/*
** 函数作用：打印引号未闭合导致的 EOF 错误。
** 效果：minishell: unexpected EOF looking for `"` 
** minishell: syntax error: unexpected end of file
*/
void    ms_err_eof_quote(char q)
{
    char    str[2];

    str[0] = q;
    str[1] = '\0';
    // 第一行报错
    ms_put3("minishell: unexpected EOF looking for `", str, "'\n");
    // 第二行报错
    ms_put3("minishell: syntax error: unexpected end of file\n", NULL, NULL);
}
