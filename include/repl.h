/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:20 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/28 02:37:18 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPL_H
# define REPL_H

# include "../libft/libft.h"
# include "build_in.h"
# include "error.h"
# include "exec.h"
# include "expander.h"
# include "lexer.h"
# include "parse.h"
# include "signals.h"
# include <readline/history.h>
# include <readline/readline.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_ast		ast;
typedef struct s_minishell	t_minishell;

/* 作用：REPL 主循环入口 */
void						repl_loop(t_minishell *ms);

/* 作用：读取一行（根据是否续行决定提示符） */
char						*repl_read(char *acc);

/* 作用：把 line 拼到 acc 里（函数内部会 free(line)） */
int							repl_join(char **acc, char *line);

/* 作用：判断字符串是否含“非空白字符”，用于决定要不要 add_history */
int							repl_has_text(const char *s);

/* 作用：把 acc 交给 lexer/parser/expander/exec，并在需要时清理 acc */
void						repl_run_acc(t_minishell *ms, char **acc);

/* 作用：单步处理一次输入（读行+拼接+run），返回 1 表示退出 */
int							repl_step(t_minishell *ms, char **acc);

/*
** 你们执行入口如果不是 exec_ast，请改这里对应的声明与 repl_run.c 的调用。
** 返回值可忽略（你们也可以用它更新 last_exit_status）。
*/
int							exec_ast(t_minishell *ms, ast *root);

#endif
