/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+         +#+        */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"
#include "minishell.h"

/* remove_quotes_flag 由 lexer 模块提供，这里只做前置声明以避免引入 t_lexer */
char    *remove_quotes_flag(const char *s, int *had_q, int *q_s, int *q_d);

/*
** 工具：strip_all_quotes_dup
** 做什么：调用 remove_quotes_flag(s, ...) 去掉外层引号；
**         如果本来没有引号，则返回 s 的副本。
** 说明：保持为非 static 以便其它模块需要时可复用（可不导出也行）。
*/
char    *strip_all_quotes_dup(const char *s, int *had_q, int *q_s, int *q_d)
{
    char    *clean;

    clean = remove_quotes_flag(s, had_q, q_s, q_d);
    if (!clean)
        return (ft_strdup(s));
    return (clean);
}
