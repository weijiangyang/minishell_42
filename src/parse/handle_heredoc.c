/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:44 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:18:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/parse.h"
#include "../../libft/libft.h"
#include "../../include/expander.h"
#include "../../include/error.h"
#include <termios.h>

volatile sig_atomic_t g_signal; // 唯一全局变量

typedef struct s_saved_signals
{
    struct sigaction sigint;
    struct sigaction sigquit;
    struct sigaction sigtstp;
} t_saved_signals;

static void save_signals(t_saved_signals *old)
{
    sigaction(SIGINT, NULL, &old->sigint);
    sigaction(SIGQUIT, NULL, &old->sigquit);
    sigaction(SIGTSTP, NULL, &old->sigtstp);
}

static void ignore_heredoc_signals(void)
{
    struct sigaction sa_ignore;
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;

    sigaction(SIGINT, &sa_ignore, NULL);
    sigaction(SIGQUIT, &sa_ignore, NULL);
    sigaction(SIGTSTP, &sa_ignore, NULL);
}
static void restore_signals(t_saved_signals *old)
{
    sigaction(SIGINT, &old->sigint, NULL);
    sigaction(SIGQUIT, &old->sigquit, NULL);
    sigaction(SIGTSTP, &old->sigtstp, NULL);
}

/* SIGINT handler */
void sigint_heredoc(int sig)
{
    (void)sig;
    g_signal = SIGINT;
}

static void setup_heredoc_signals(void)
{
    struct sigaction sa;

    // SIGINT
    sa.sa_handler = sigint_heredoc;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // SIGTSTP
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &sa, NULL);

    // SIGQUIT
    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
}

/* heredoc_loop: canonical 模式，和 bash 行为一致 */
static int heredoc_loop(int write_fd, const char *delimiter, t_minishell *msh, int quoted)
{
    char *line;
    char *full_line = NULL; // 用于拼接没有换行符的片段
    while (1)
    {
        // 只有当缓冲区为空时，才打印提示符
        if (full_line == NULL)
            write(STDOUT_FILENO, "heredoc> ", 9);

        g_signal = 0;
        line = get_next_line(STDIN_FILENO);

        // --- 处理 Ctrl+C ---
        if (g_signal == SIGINT)
        {
            free(line);
            free(full_line);
            write(1, "\n", 1);
            return -1;
        }

        // --- 处理 Ctrl+D ---
        if (!line)
        {
            if (full_line == NULL)
            { // 纯空行按 Ctrl+D
                printf("bash: warning: ... (wanted '%s')\n", delimiter);
                break;
            }
            // 如果有残留内容按 Ctrl+D，Bash 会将其视为一行处理
            line = full_line;
            full_line = NULL;
        }
        else if (full_line)
        { // 如果之前有没写完的片段，拼接起来
            char *tmp = ft_strjoin(full_line, line);
            free(full_line);
            free(line);
            line = tmp;
        }

        // --- 判断是否读到了完整的一行 ---
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0'; // 去掉换行符
            if (strcmp(line, delimiter) == 0)
            {
                free(line);
                break;
            }
            if (!quoted)
                line = expand_heredoc_vars(msh, line);
            write(write_fd, line, strlen(line));
            write(write_fd, "\n", 1);
            free(line);
            full_line = NULL; // 清空暂存，下次循环会打印提示符
        }
        else
        {
            // 没有换行符，说明用户按了 Ctrl+D
            // 我们暂存这段文字，进入下一次 read，不打印提示符
            full_line = line;
        }
    }
    return 0;
}

int handle_heredoc(t_redir *new_redir, t_minishell *shell)
{
    int pipefd[2];
    pid_t pid;
    int status;
    int result;

    t_saved_signals saved;

    save_signals(&saved);

    if (pipe(pipefd) < 0)
    {
        restore_signals(&saved);
        return -1;
    }

    pid = fork();
    if (pid < 0)
    {
        restore_signals(&saved);
        return -1;
    }
    if (pid == 0)
    {
        setup_heredoc_signals();
        close(pipefd[0]);
        if (new_redir->quoted)
            result = heredoc_loop(pipefd[1], new_redir->filename, shell, 1);
        else
            result = heredoc_loop(pipefd[1], new_redir->filename, shell, 0);
        if (result < 0)
            exit(130);
        close(pipefd[1]);
        exit(0);
    }
    else
    {
        close(pipefd[1]);
        ignore_heredoc_signals();
        while (waitpid(pid, &status, 0) == -1)
        {
            if (errno != EINTR)
            {
                restore_signals(&saved);
                return -1; // 等待失败
            }
        }
        restore_signals(&saved);
        if (WIFEXITED(status))
        {
            int code = WEXITSTATUS(status);

            if (code == 0)
            {
                // heredoc 正常完成
                new_redir->heredoc_fd = pipefd[0];
                return 0;
            }
            else
            {
                // Ctrl+C (130) 或其他 exit
                close(pipefd[0]);
                new_redir->heredoc_fd = -1;
                shell->last_exit_status = code;
                return -1;
            }
        }
        else if (WIFSIGNALED(status))
        {
            close(pipefd[0]);
            new_redir->heredoc_fd = -1;
            shell->last_exit_status = 128 + WTERMSIG(status);
            return -1;
        }
        new_redir->heredoc_fd = pipefd[0];
        return 0;
    }
}
