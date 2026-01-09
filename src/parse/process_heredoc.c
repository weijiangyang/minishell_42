/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:15:42 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:15:47 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"
#include "parse.h"
#include "expander.h"
/**
 * @brief 将新读取的一行合并到已有的总字符串中，并自动管理内存。
 * * 该函数实现了字符串的动态累加逻辑：
 * 1. 如果当前总行 (full_line) 为空，直接返回新行作为起始内容。
 * 2. 否则，使用 ft_strjoin 将新行拼接在旧内容之后。
 * 3. 释放传入的两个旧指针 (full_line 和 line)，防止内存泄漏。
 * 4. 返回拼接后的新字符串指针。
 * * @param full_line 已经累积读取的所有行字符串指针。
 * @param line      当前新读取的一行字符串指针。
 * @return char* 返回拼接后的新字符串指针。
 */
static char *join_full_line(char *full_line, char *line)
{
    char *tmp;

    if (!full_line)
        return line;
    tmp = ft_strjoin(full_line, line);
    free(full_line);
    free(line);
    return tmp;
}

/**
 * @brief 处理 Heredoc 中读取的每一行数据。
 * * 该函数实现了以下逻辑：
 * 1. 换行符检查：如果行末包含 '\n'，说明这是一行完整的输入。
 * 2. 终止符匹配：去除换行符后，检查内容是否与 delimiter 相同。若匹配，则结束读取。
 * 3. 变量展开：如果 delimiter 未被引号包裹 (!quoted)，则对该行进行环境变量展开。
 * 4. 数据写入：将处理后的行内容写入管道，并补回换行符。
 * 5. 缓冲区管理：如果当前行不完整（没有 '\n'），则将其暂存在 full_line 中等待下次拼接。
 * * @param line       指向当前读取内容的指针地址。
 * @param full_line  用于暂存不完整行的缓冲区指针地址。
 * @param delimiter  Heredoc 的结束标志字符串。
 * @param write_fd   管道的写端，用于传输数据给父进程。
 * @param msh        全局上下文，用于获取环境变量。
 * @param quoted     标志位，若为真则跳过变量展开。
 * @return int       匹配到终止符返回 1；处理完毕或行未结束返回 0。
 */
static int process_heredoc_line(char **line, char **full_line,
                                const char *delimiter, int write_fd, t_minishell *msh, int quoted)
{
    size_t len;

    len = strlen(*line);
    if (len > 0 && (*line)[len - 1] == '\n')
    {
        (*line)[len - 1] = '\0';
        if (strcmp(*line, delimiter) == 0)
        {
            free(*line);
            *full_line = NULL;
            return 1;
        }
        if (!quoted)
            *line = expand_heredoc_vars(msh, *line);
        write(write_fd, *line, strlen(*line));
        write(write_fd, "\n", 1);
        free(*line);
        *full_line = NULL;
        return 0;
    }
    *full_line = *line;
    return 0;
}

/**
 * @brief 读取并处理逻辑。
 * * 该函数是读取循环的核心，执行以下流程：
 * 1. 使用 get_next_line 从标准输入读取数据。（“为什么 heredoc 不用 readline？“因为 heredoc 是阻塞 IO 模式，不需要行编辑和历史功能，使用 get_next_line 可以避免 readline 内部信号逻辑带来的副作用，
        同时更容易精确控制 Ctrl-C 行为。”）。
 * 2. 信号处理：检测全局变量 g_signal。如果捕获到 SIGINT (Ctrl+C)，清理内存并返回中断信号。
 * 3. EOF 处理：如果读取结束且无残留数据，返回结束标志。
 * 4. 拼接逻辑：如果存在之前暂存的不完整行 (*full_line)，则将其与新读入的行拼接。
 * 5. 状态分发：调用 process_heredoc_line 进行终止符匹配、变量展开和写入。
 * * @param full_line  指向跨行暂存缓冲区的指针地址。
 * @param delimiter  Heredoc 终止符。
 * @param write_fd   管道写端文件描述符。
 * @param msh        全局上下文结构体。
 * @param quoted     终止符是否带引号（决定是否展开变量）。
 * @return int       0: 继续读取; -1: 被 SIGINT 中断; 1: 读取到 EOF; 2: 匹配到终止符。
 */
static int read_and_process_line(char **full_line, const char *delimiter,
                                 int write_fd, t_minishell *msh, int quoted)
{
    char *line;

    line = get_next_line(STDIN_FILENO);
    if (g_signal == SIGINT)
    {
        free(line);
        free(*full_line);
        write(1, "\n", 1);
        return -1;
    }
    if (!line && *full_line == NULL)
    {
        write(1, "\n", 1);
        ft_putchar_fd("minishell: warning: here-document at line ", 2);
        
        char *b = "delimited by end-of-file('want of )";
        const char *c = delimiter;

        ms_put3(a, b,c);
        return 1;
    }
        
    if (!line)
    {
        line = *full_line;
        *full_line = NULL;
    }
    else if (*full_line)
        line = join_full_line(*full_line, line);
    if (process_heredoc_line(&line, full_line, delimiter, write_fd, msh, quoted))
        return 2;
    return 0;
}

/**
 * @brief 执行 Heredoc 读取的主循环。
 * * 该函数保持程序在输入模式，直到匹配终止符或被中断：
 * 1. 初始化信号处理：调用 setup_heredoc_signals 设置 Heredoc 专用的信号行为。
 * 2. 交互提示：当没有暂存的行碎片时，向标准输出打印 "heredoc> " 提示符。
 * 3. 驱动读取：调用 read_and_process_line 获取并处理输入。
 * 4. 状态判定：
 * - 返回 -1：表示捕获到 SIGINT (Ctrl+C)，立即终止并返回失败。
 * - 返回其他非零值 (1 或 2)：表示读取结束（EOF 或 匹配终止符），返回成功。
 * * @param write_fd   管道的写端，用于发送数据。
 * @param delimiter  用户定义的结束字符串（如 "EOF"）。
 * @param msh        全局上下文，用于变量展开和状态管理。
 * @param quoted     标志位，决定是否对输入内容进行变量展开。
 * @return int       正常结束返回 0；被信号中断返回 -1。
 */
int heredoc_loop(int write_fd, const char *delimiter,
                        t_minishell *msh, int quoted)
{
    char *full_line;
    int status;

    full_line = NULL;
    setup_heredoc_signals();
    while (1)
    {
        if (!full_line)
            write(STDOUT_FILENO, "heredoc> ", 9);
        g_signal = 0;
        status = read_and_process_line(&full_line, delimiter,
                                       write_fd, msh, quoted);
        if (status != 0)
        {
            if(status == -1)
                return (-1);
            return (0);
        }
    }
}

