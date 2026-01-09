/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_signal.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:19:41 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/09 15:13:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"
#include "signals.h"

volatile sig_atomic_t g_signal;

/**
 * @brief 备份 Shell 当前的信号处理行为。
 * * 使用 sigaction 获取当前系统中针对特定信号的配置：
 * 1. SIGINT (Ctrl+C): 记录当前的交互式中断行为。
 * 2. SIGQUIT (Ctrl+\): 记录当前的退出行为（Shell 通常忽略此信号）。
 * 3. SIGTSTP (Ctrl+Z): 记录当前的暂停行为。
 * * @param old 指向 t_saved_signals 结构体的指针，用于存储旧的信号配置。
 */
void save_signals(t_saved_signals *old)
{
    sigaction(SIGINT, NULL, &old->sigint);
    sigaction(SIGQUIT, NULL, &old->sigquit);
    sigaction(SIGTSTP, NULL, &old->sigtstp);
}

/**
 * @brief 将核心信号设置为忽略模式。
 * * 该函数通过 sigaction 配置 SIG_IGN (Ignore)：
 * 1. SIGINT (Ctrl+C): 忽略中断信号，防止父进程在等待子进程输入时被意外杀死。
 * 2. SIGQUIT (Ctrl+\): 忽略退出信号，符合 Shell 的标准交互行为。
 * 3. SIGTSTP (Ctrl+Z): 忽略挂起信号，防止父进程在关键 IO 阶段进入后台。
 * * 这样做确保了信号只会由正在运行的子进程（Heredoc 处理进程）独立接收和处理。
 */
void ignore_heredoc_signals(void)
{
    struct sigaction sa_ignore;

    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    sigaction(SIGINT, &sa_ignore, NULL);
    sigaction(SIGQUIT, &sa_ignore, NULL);
    sigaction(SIGTSTP, &sa_ignore, NULL);
}

/**
 * @brief 还原之前备份的信号处理配置。
 * * 该函数将信号处理器重置为 save_signals 捕获的状态：
 * 1. 恢复 SIGINT: 重新启用主 Shell 的交互式中断逻辑。
 * 2. 恢复 SIGQUIT: 恢复对退出信号的原始处理（通常是忽略）。
 * 3. 恢复 SIGTSTP: 恢复对暂停信号的原始处理。
 * * 这通常在子进程结束、Heredoc 读取完毕或阻塞式命令执行完成后调用，
 * 以确保主 readline 循环能够恢复正常的终端交互行为。
 * * @param old 指向包含备份配置的 t_saved_signals 结构体的指针。
 */
void restore_signals(t_saved_signals *old)
{
    sigaction(SIGINT, &old->sigint, NULL);
    sigaction(SIGQUIT, &old->sigquit, NULL);
    sigaction(SIGTSTP, &old->sigtstp, NULL);
}

/**
 * @brief Heredoc 模式下的 SIGINT (Ctrl+C) 处理器。
 * * 1. (void)sig: 显式忽略参数，防止编译器产生“未使用的变量”警告。
 * 2. 状态标记: 将全局变量 g_signal 设置为 SIGINT。
 * * 这种处理方式允许 Heredoc 的读取循环（read_and_process_line）
 * 通过 g_signal 的值来检测用户是否发出了中断请求，
 * 从而清理内存并退出输入模式，而不是让程序崩溃。
 */
static void sigint_heredoc(int sig)
{
    (void)sig;
    g_signal = SIGINT;
}

/**
 * @brief 配置 Heredoc 模式专用的信号处理器。
 * * 该函数重新定义了三个核心信号的行为：
 * 1. SIGINT (Ctrl+C): 绑定到 sigint_heredoc。当触发时，它会设置全局标志位
 * g_signal，允许读取循环识别中断并退出。
 * 2. SIGTSTP (Ctrl+Z): 设置为 SIG_IGN (忽略)。防止用户在 Heredoc 输入期间
 * 将 Shell 挂起到后台，这会导致复杂的终端状态恢复问题。
 * 3. SIGQUIT (Ctrl+\): 设置为 SIG_IGN (忽略)。minishell 的标准行为是在 
 * Heredoc 提示符下不产生 Core Dump 也不退出。
 */
void setup_heredoc_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = sigint_heredoc;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &sa, NULL);
    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
}
