/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by ***********       #+#    #+#             */
/*   Updated: 2025/12/21 18:21:01 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H


# include <stddef.h>
# include <stdlib.h>
typedef struct s_ast ast;
typedef struct s_minishell	t_minishell;

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

#endif
