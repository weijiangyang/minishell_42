/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:35 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/09 15:13:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

/**
 * @brief 处理交互模式下的 SIGINT (Ctrl+C) 信号。
 * * 该函数执行以下 Readline 状态重置操作：
 * 1. write(1, "\n", 1): 在终端打印一个物理换行符。
 * 2. rl_on_new_line(): 告知 Readline 内部状态：光标已经移到了新行。
 * 3. rl_replace_line("", 0): 清空当前 Readline 的输入缓冲区（抹除已输入的文字）。
 * 4. rl_redisplay(): 强制刷新屏幕，在当前位置重新绘制提示符 (Prompt)。
 * 5. g_signal = SIGINT: 记录全局信号标志，供解析器或执行器判断状态。
 * 6. rl_done = 1: 关键属性，告知 rl_getc 立即结束当前的阻塞等待，
 * 使 readline() 函数返回一个 NULL 指针或空字符串。
 */
void sigint_prompt(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    g_signal = SIGINT;
    rl_done = 1;
}

/**
 * @brief 在子进程中恢复信号的默认系统行为。
 * * 在执行 fork() 之后且执行 execve() 之前调用：
 * 1. SIGINT (Ctrl+C): 恢复为 SIG_DFL (Default)。允许用户通过中断信号杀死运行中的外部程序。
 * 2. SIGQUIT (Ctrl+\): 恢复为 SIG_DFL。允许外部程序在接收到退出信号时产生 Core Dump 并终止。
 * 3. SIGTSTP (Ctrl+Z): 恢复为 SIG_DFL。允许外部程序响应挂起信号进入后台。
 * * 这一步必不可少，因为子进程会继承父进程的信号掩码和处理函数。如果不重置，
 * 外部程序将继承主 Shell 忽略 SIGQUIT 或捕获 SIGINT 的逻辑，导致其无法被正常控制。
 */
void setup_child_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

/**
 * @brief 在命令执行期间将父进程的信号设置为忽略模式。
 * * 当 Minishell fork 出子进程并等待其结束时：
 * 1. SIGINT (Ctrl+C): 设置为 SIG_IGN。父进程不会因 Ctrl+C 退出，
 * 只有正在运行的子进程（如 cat, top）会响应此信号并终止。
 * 2. SIGQUIT (Ctrl+\): 设置为 SIG_IGN。父进程不响应退出信号，
 * 由子进程负责决定是否产生 Core Dump。
 * 3. SIGTSTP (Ctrl+Z): 设置为 SIG_IGN。防止父进程在等待子进程时被挂起。
 * * 这样做确保了终端的控制逻辑清晰：信号被直接发送给前台进程组（子进程），
 * 而父进程保持静默，直到 waitpid 获取到子进程的退出状态。
 */
void setup_parent_exec_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
}
/**
 * @brief 配置主交互模式下的信号处理。
 * * 该函数设置 Shell 在等待用户输入时的标准行为：
 * 1. SIGINT (Ctrl+C): 绑定到 sigint_prompt。触发时通常执行：
 * - 打印新行 (\n)
 * - 告知 readline 重新显示提示符
 * - 设置退出状态码为 130
 * 2. SIGQUIT (Ctrl+\): 设置为 SIG_IGN (忽略)。
 * - 按照 minishell 规范，在主提示符下按下 Ctrl+\ 不应产生 Core Dump 或退出 Shell。
 * * 这一配置保证了 Shell 的交互体验符合 POSIX 标准。
 */
void setup_prompt_signals(void)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigint_prompt;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
}

