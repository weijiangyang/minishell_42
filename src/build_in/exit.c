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

/**
 * @brief exit 内置命令的完整实现。
 * * 行为逻辑：
 * 1. 终端提示：如果是在交互式终端且没有管道，打印 "exit"。
 * 2. 参数校验：
 * - 非数字参数：如果第一个参数不是数字，报错并以状态码 2 退出。
 * - 参数过多：如果有超过一个参数，报错并不退出（保持 Shell 运行）。
 * 3. 状态码转换：将字符串参数转换为 long，并截断为 unsigned char (0-255)。
 * 4. 默认行为：若无参数，则以最近一次命令的退出状态 (last_exit_status) 退出。
 * * @param argv 命令参数数组。
 * @param msh  全局上下文，用于获取最后的退出状态。
 * @return int 仅在参数过多不退出时返回 1。
 */
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
