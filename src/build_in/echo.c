/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:34 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:16:37 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/build_in.h"

int ft_echo(char **argv)
{
    int i;
    int print_newline;
    int j;

    i = 1;
    print_newline = 1;
    // 1. 解析选项：处理 -n, -nnnn, -n -n 等情况
    while (argv[i] && argv[i][0] == '-')
    {
        j = 1;
        // 检查是否全是 'n'，例如 "-nnnn"
        if (argv[i][j] == '\0') // 只有一个 "-" 则不是选项
            break;
        while (argv[i][j] == 'n')
            j++;
        // 如果遇到了非 'n' 字符且不是结尾，说明不是合法选项（例如 -nx）
        if (argv[i][j] != '\0')
            break;

        // 如果走到这里，说明是合法的 -n(nnn) 选项
        print_newline = 0;
        i++;
    }

    // 2. 打印剩余参数
    while (argv[i])
    {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" ");
        i++;
    }

    // 3. 打印换行符
    if (print_newline)
        printf("\n");
    return (0);
}
