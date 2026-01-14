/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:18:44 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"
#include "parse.h"

/**
 * @brief 在子进程中处理 Heredoc 的输入逻辑。
 * * 该函数在 fork 后的子进程中运行：
 * 1. 设置专门用于 Heredoc 输入模式的信号处理（如拦截 Ctrl+C）。
 * 2. 关闭管道的读端 (pipefd[0])，仅保留写端。
 * 3. 根据是否带引号 (redir->quoted) 调用 heredoc_loop，决定是否进行变量展开。
 * 4. 如果输入过程被信号中断（result < 0），以状态码 130 退出。
 * 5. 完成后关闭写端并正常退出子进程。
 * * @param redir   指向当前的重定向节点，包含 Heredoc 的终止符和引用状态。
 * @param shell   指向全局结构体，用于获取环境变量（用于展开）。
 * @param pipefd  已打开的管道文件描述符数组。
 */
static void heredoc_fork_child(t_redir *redir, t_minishell *shell,
							   int pipefd[2])
{
	int result;

	setup_heredoc_signals();
	close(pipefd[0]);
	if (redir->quoted)
		result = heredoc_loop(pipefd[1], redir->filename, shell, 1);
	else
		result = heredoc_loop(pipefd[1], redir->filename, shell, 0);
	if (result < 0)
	{
		close(pipefd[1]);
		ms_child_exit(shell, shell->cur_ast, 130);
	}
	close(pipefd[1]);
	ms_child_exit(shell, shell->cur_ast, 0);
}

/**
 * @brief 等待子进程结束。
 * * 该函数封装了 waitpid，核心功能是处理被信号中断的情况：
 * 1. 在循环中调用 waitpid。
 * 2. 如果 waitpid 返回 -1 且 errno 为 EINTR，表示调用被信号中断，应继续尝试等待。
 * 3. 如果 errno 为其他错误（如 ECHILD，找不到子进程），则返回错误。
 * * @param pid    要等待的子进程 PID。
 * @param status 指向存储退出状态信息的整数指针。
 * @return int   成功返回 0；发生非中断类的系统错误返回 -1。
 */
static int wait_for_child(pid_t pid, int *status)
{
	while (waitpid(pid, status, 0) == -1)
	{
		if (errno != EINTR)
			return (-1);
	}
	return (0);
}

/**
 * @brief 处理 Heredoc 子进程的退出状态并管理资源。
 * * 该函数根据子进程的结束方式执行逻辑：
 * 1. 正常退出 (WIFEXITED)：
 * - 状态码为 0：表示输入成功，将管道读端赋值给 redir->heredoc_fd。
 * - 状态码非 0：表示输入失败（如语法错），关闭读端并更新 Shell 退出状态。
 * 2. 信号终止 (WIFSIGNALED)：
 * - 通常指 Ctrl+C。关闭读端，按 minishell 规范计算状态码 (128 + 信号值)。
 * * @param status  由 waitpid 获取的子进程退出状态信息。
 * @param pipefd  Heredoc 通讯管道。
 * @param redir   当前的重定向节点，用于存储结果 fd。
 * @param shell   指向全局结构体，用于更新最后一次执行的状态码。
 * @return int    成功返回 0；失败或被信号中断返回 -1。
 */
static int handle_child_exit(int status, int pipefd[2], t_redir *redir,
							 t_minishell *shell)
{
	int code;

	if (WIFEXITED(status))
	{
		code = WEXITSTATUS(status);
		if (code == 0)
		{
			redir->heredoc_fd = pipefd[0];
			return (0);
		}
		close(pipefd[0]);
		redir->heredoc_fd = -1;
		shell->last_exit_status = code;
		return (-1);
	}
	else if (WIFSIGNALED(status))
	{
		close(pipefd[0]);
		redir->heredoc_fd = -1;
		shell->last_exit_status = 128 + WTERMSIG(status);
		return (-1);
	}
	redir->heredoc_fd = pipefd[0];
	return (0);
}

/**
 * @brief 在父进程中管理 Heredoc 子进程的同步与回收。
 * * 该函数在 fork 之后执行以下操作：
 * 1. 关闭管道写端 (pipefd[1])：父进程仅负责读取子进程传回的数据。
 * 2. 信号保护：在等待期间忽略特定的信号，避免父进程被意外中断。
 * 3. 阻塞等待：调用 wait_for_child 等待子进程完成输入。
 * 4. 状态结算：调用 handle_child_exit 解析退出状态并决定是否保留管道数据。
 * * @param pid     Heredoc 子进程的 PID。
 * @param pipefd  用于与子进程通信的管道。
 * @param redir   当前的重定向节点，用于存储最终的读端 fd。
 * @param shell   指向全局结构体，用于获取/更新环境变量及状态。
 * @return int    成功且完成输入返回 0；子进程出错或被信号中断返回 -1。
 */
static int heredoc_parent(pid_t pid, int pipefd[2], t_redir *redir,
						  t_minishell *shell)
{
	int status;

	close(pipefd[1]);
	ignore_heredoc_signals();
	if (wait_for_child(pid, &status) == -1)
		return (-1);
	return (handle_child_exit(status, pipefd, redir, shell));
}

/**
 * @brief Heredoc 处理的主入口函数。
 * * 该函数通过以下步骤协调 Heredoc 的读取过程：
 * 1. 备份当前的信号处理配置 (t_saved_signals)。
 * 2. 创建用于子进程传递输入数据的管道 (pipe)。
 * 3. 创建子进程 (fork)：
 * - 子进程：调用 heredoc_fork_child 处理用户输入流。
 * - 父进程：调用 heredoc_parent 等待输入结束并回收资源。
 * 4. 无论成功与否，最后都会恢复原始的信号处理配置。
 * * @param redir 指向当前需要处理的重定向节点。
 * @param shell 指向全局上下文结构体。
 * @return int   成功读取并准备好 fd 返回 0；发生错误或被中断返回 -1。
 */
int handle_heredoc(t_redir *redir, t_minishell *shell)
{
	int pipefd[2];
	pid_t pid;
	t_saved_signals saved;
	int ret;

	save_signals(&saved);
	if (pipe(pipefd) < 0)
		return (restore_signals(&saved), -1);
	pid = fork();
	if (pid < 0)
		return (restore_signals(&saved), -1);
	if (pid == 0)
		return (heredoc_fork_child(redir, shell, pipefd), 0);
	else
	{
		ret = heredoc_parent(pid, pipefd, redir, shell);
		restore_signals(&saved);
		return (ret);
	}
}
