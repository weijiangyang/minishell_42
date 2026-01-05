/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:19:35 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:19:37 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/signals.h"

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

void setup_prompt_signals(void)
{
    signal(SIGINT, sigint_prompt);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}

void setup_child_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

void setup_parent_exec_signals(void)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}



void setup_signals(void)
{
    struct sigaction sa;

    // 1. 指定处理函数
    sa.sa_handler = sigint_prompt;

    // 2. 初始化信号集，确保在处理 SIGINT 时不会被其他信号干扰
    sigemptyset(&sa.sa_mask);

    // 3. 设置标志位
    // SA_RESTART: 让被信号中断的系统调用（如 read）自动重启，避免 readline 异常退出
    sa.sa_flags = 0;

    // 4. 应用配置
    // 绑定 Ctrl+C (SIGINT)
    sigaction(SIGINT, &sa, NULL);

    // 5. 忽略 Ctrl+\ (SIGQUIT)
    // 这是 Shell 的标准行为：在主提示符下按下 Ctrl+\ 不应有任何反应
    struct sigaction sa_ignore;
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    sigaction(SIGQUIT, &sa_ignore, NULL);
}
