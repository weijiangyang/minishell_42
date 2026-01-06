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

#include "minishell.h"
#include "build_in.h"

/**
 * @brief 解析 echo 命令的选项标志（主要是 -n）。
 * * 行为逻辑：
 * 1. 标志识别：跳过所有符合格式的 -n 选项。支持合并形式，如 `-nnnn`。
 * 2. 严格校验：
 * - 如果参数仅为 `-`，则视其为普通字符串，停止解析。
 * - 如果参数包含 'n' 以外的字符（如 `-nx`），则该参数及其后内容均视为普通字符串。
 * 3. 换行控制：若检测到至少一个有效的 `-n`，则取消末尾的换行符。
 * 4. 索引偏移：通过 start_index 返回第一个非标志参数的位置。
 * * @param argv        命令参数数组。
 * @param start_index [输出参数] 记录第一个待打印字符串的下标。
 * @return int         是否需要打印换行符（1: 需要, 0: 不需要）。
 */
static int parse_echo_flags(char **argv, int *start_index)
{
    int i;
    int j;
    int print_newline;

    i = 1;
    print_newline = 1;
    while (argv[i] && argv[i][0] == '-')
    {
        j = 1;
        if (argv[i][j] == '\0')
            break ;
        while (argv[i][j] == 'n')
            j++;
        if (argv[i][j] != '\0')
            break ;
        print_newline = 0;
        i++;
    }
    *start_index = i;
    return print_newline;
}

/**
 * @brief 打印从指定索引开始的所有 echo 参数。
 * * 打印规则：
 * 1. 连续输出：从 argv[start_index] 开始遍历，直到遇到 NULL 指针。
 * 2. 空格处理：在当前参数之后检查是否存在下一个参数（argv[i + 1]），
 * 如果存在，则打印一个空格作为分隔符。
 * 3. 边界控制：最后一个参数之后不会打印空格，这保证了输出的精确性，
 * 尤其是在处理重定向或结合 -n 标志时。
 * * @param argv        命令参数数组。
 * @param start_index  第一个非标志（non-flag）待打印参数的下标。
 */
static void echo_print_args(char **argv, int start_index)
{
    int i = start_index;

    while (argv[i])
    {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" ");
        i++;
    }
}

/**
 * @brief echo 内置命令的完整实现。
 * * 执行流程：
 * 1. 选项解析：调用 parse_echo_flags 识别所有的 -n 标志。
 * - 如果有有效的 -n，则 print_newline 设为 0。
 * - start_index 会指向第一个需要打印的实际文本参数。
 * 2. 内容输出：调用 echo_print_args 遍历 argv，按顺序打印字符串并处理中间空格。
 * 3. 换行处理：如果解析阶段没有发现 -n，则在末尾补一个 '\n'。
 * 4. 状态返回：按照 POSIX 标准，echo 几乎总是返回 0 (成功)。
 * * @param argv 命令参数数组（argv[0] 为 "echo"）。
 * @return int 始终返回 0。
 */
int ft_echo(char **argv)
{
    int print_newline;
    int start_index;

    print_newline = parse_echo_flags(argv, &start_index);
    echo_print_args(argv, start_index);
    if (print_newline)
        printf("\n");
    return 0;
}
