/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:20 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 22:34:17 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPL_H
#define REPL_H

#include "../libft/libft.h"
#include "build_in.h"
#include "error.h"
#include "exec.h"
#include "expander.h"
#include "lexer.h"
#include "parse.h"
#include "signals.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_ast t_ast;
typedef struct s_minishell t_minishell;

/*
** 作用：REPL 主循环入口。
** 说明：题目要求 minishell 不接收命令行参数，所以这里不需要“argv 先执行”。
**       续行（比如引号没闭合）由内部的 acc 变量自己管理。
*/
void repl_loop(t_minishell *ms);

/*
** 作用：读取一行输入。若 acc 非空，说明在“续行模式”，提示符用 "> "。
** 注意：这里不写 history，history 只在“整条命令完整后”才写入。
*/
char *repl_read(char *acc);

/*
** 作用：把本次输入 line 拼到累计输入 acc 后面。
** 规则：如果 acc 已经有内容，先加一个 '\n' 再拼 line（模拟多行输入）。
** 返回：成功 1，失败 0（失败一般是 malloc 失败）。
*/
int repl_join(char **acc, char *line);

/* 作用：判断字符串里是否有“非空白字符”。有则返回 1，否则 0。 */
int repl_has_text(const char *s);

/* 作用：安全释放累计输入 acc，并把指针置 NULL。 */
void repl_free_acc(char **acc);

/* 作用：拿着 acc 去做 lexer/parse/exec；需要续行时会保留 acc 不执行。 */
void repl_run_acc(t_minishell *ms, char **acc);

/*
** 作用：REPL 的“一步”：读一行→拼接→尝试执行。
** 返回：1 表示要退出 shell；0 表示继续循环。
*/
int repl_step(t_minishell *ms, char **acc);


/* repl_utils.c */
int		lexer_has_heredoc(t_lexer *lx);
char	*dup_first_line(const char *s);

#endif
