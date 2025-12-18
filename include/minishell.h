/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 15:21:40 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 15:57:30 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"

# include "error.h"
# include "exec.h"
# include "expander.h"
# include "lexer.h"
# include "parse.h"
# include "signals.h"
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_signal;

typedef struct s_minishell
{
	// lexer
	t_lexer						*lexer;
	char						*args;
	char						lexer_unclosed_quote;
	/* 0 或 '\'' 或 '"'：当 handle_lexer 返回 NEED_MORE 时有效 */
	int lexer_need_more; /* 0/1：可选调试字段，表示上次词法需要更多输入 */

	char *raw_line; // 原始输入行

	int n_pipes; // 管道 “|” 的个数（cmd 数 - 1）

	int last_exit_status; // 上一条命令退出状态（用于 $? 扩展）
	char **envp;          /* execve 用的数组 */
	t_env *env;           /* 你们 builtin 用的链表 */

	char						**paths;

	// loop
}								t_minishell;

#endif
