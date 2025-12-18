/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

# include "../../libft/libft.h"
# include <stddef.h>
# include <stdlib.h>
# include "parse.h"

/* 前置声明 */
typedef struct s_minishell  t_minishell;

// /* ===== AST（按你给定的结构，不增字段） ===== */
// typedef enum e_node_type
// {
//     NODE_CMD,
//     NODE_PIPE,
//     NODE_AND,
//     NODE_OR,
//     NODE_SUBSHELL,
//     NODE_BACKGROUND,
//     NODE_SEQUENCE
// }   node_type;

// typedef struct s_ast
// {
//     node_type       type;
//     /* node_cmd 有效字段 */
//     char            **argv;
//     char            *redir_in;
//     char            *redir_out;
//     char            *redir_append;
//     char            *heredoc_delim;
//     int             n_pipes;
//     /* 组合节点 */
//     struct s_ast    *left;
//     struct s_ast    *right;
//     /* 子shell */
//     struct s_ast    *sub;
//     int             is_background;
//     int             heredoc_quoted; /* 1 表示分隔符曾被引号包裹 */
// }   ast;

/* ===== 引号状态机（原样保留） ===== */
enum                        qstate
{
    Q_NONE = 0,
    Q_SQ = 1,
    Q_DQ = 2
};

typedef struct s_exp_data
{
    t_minishell             *minishell;
    char                    **out;
}                           t_exp_data;

/* ===== 新：基于 AST 的展开入口 ===== */
int                         expander_ast(t_minishell *minishell, ast *root);

/* ===== 其余原有 API（保持不变） ===== */
char                        *expander_str(t_minishell *minishell, char *str);
int                         scan_expand_one(t_exp_data *data, const char *s,
                                int j, enum qstate q);
char                        *expand_all(t_minishell *minishell,
                                const char *str);

int                         is_name_start(int c);
int                         is_name_char(int c);
int                         var_len(const char *s);
char                        *env_value_dup(t_minishell *minishell,
                                const char *name, int len);

char                        *str_join_free(char *a, const char *b);
size_t                      equal_sign(char *str);

#endif
