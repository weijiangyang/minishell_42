/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by ***********       #+#    #+#             */
/*   Updated: 2025/12/21 22:36:06 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>
#include <termios.h>

#include "parse.h"


enum						qstate
{
	Q_NONE = 0,
	Q_SQ = 1,
	Q_DQ = 2
};

typedef struct s_exp_data
{
	t_minishell				*minishell;
	char					**out;
}							t_exp_data;

/*
** 函数作用：展开入口，parse 之后 exec 之前调用，遍历整棵 AST 做展开。
** 参数：minishell(全局上下文), root(AST 根)
*/
int							expander_ast(t_minishell *minishell, ast *root);

/*
** 函数作用：只展开一个 CMD 节点（argv + 重定向链表）。
** 参数：msh(全局上下文), node(CMD 节点)
*/
int							expander_expand_cmd_node(t_minishell *msh,
								ast *node);

/*
** 函数作用：展开一个字符串（$ 展开 + 去引号），并释放传入的旧字符串。
** 参数：minishell(全局上下文), str(会被 free)
*/
char						*expander_str(t_minishell *minishell, char *str);

int							scan_expand_one(t_exp_data *data, const char *s,
								int j, enum qstate q);
char						*expand_all(t_minishell *minishell,
								const char *str);

int							is_name_start(int c);
int							is_name_char(int c);
int							var_len(const char *s);
char						*env_value_dup(t_minishell *minishell,
								const char *name, int len);

char						*str_join_free(char *a, const char *b);
size_t						equal_sign(char *str);

/*
** 由 lexer 模块提供：去引号并同时告诉你有没有引号。
** 这里声明一下，避免 expander 依赖 lexer.h 的结构体。
*/
char						*remove_quotes_flag(const char *s, int *had_q,
								int *q_s, int *q_d);

#endif
