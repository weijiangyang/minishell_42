/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:24:07 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:24:10 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../include/minishell.h" // 你的 t_minishell 定义
#include "../../libft/libft.h"
#include "../../include/error.h"

// 判断字符串是否为合法数字
static int is_numeric(const char *s)
{
    int i = 0;

    if (!s || !s[0])
        return 0;
    if (s[i] == '+' || s[i] == '-')
        i++;
    if (!s[i])
        return 0;
    while (s[i])
    {
        if (!ft_isdigit((unsigned char)s[i]))
            return 0;
        i++;
    }
    return 1;
}

// 把数字字符串转成 long（假设已通过 is_numeric）
static long to_long(const char *s)
{
    int i = 0;
    int sign = 1;
    long val = 0;

    if (s[i] == '+' || s[i] == '-')
    {
        if (s[i] == '-')
            sign = -1;
        i++;
    }
    while (s[i])
    {
        val = val * 10 + (s[i] - '0');
        i++;
    }
    return val * sign;
}


int builtin_exit(char **argv, t_minishell *msh)
{
    long code;

    code = 0;
    if (isatty(STDIN_FILENO) && !msh->n_pipes)
        printf("exit\n");
    if (argv && argv[1])
    {
        if (!is_numeric(argv[1]))
        {
            ms_put3("minishell: exit: ", argv[1],
                    ": numeric argument required\n");   
            exit(2);
        }
        if (argv[2])
        {
            ms_put3("minishell: exit: ", "too many arguments\n", "");
            return 1;
        }
        code = to_long(argv[1]);      
        exit((unsigned char)code); 
    }
    if (msh)
        exit(msh->last_exit_status);
    return 0;
}
