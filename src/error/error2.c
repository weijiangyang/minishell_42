/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 17:22:21 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/29 17:54:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/error.h"
#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
/*
** 函数作用：
** 打印系统调用失败的错误（带 minishell 前缀），格式尽量贴近 bash。
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
