/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 15:21:40 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 18:23:02 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include <signal.h>

/* forward declarations */
typedef struct s_lexer t_lexer;
typedef struct s_env t_env;
typedef struct s_ast t_ast;

extern volatile sig_atomic_t g_signal;

typedef struct s_minishell
{
	t_lexer *lexer;
	t_env *env;
	t_ast *cur_t_ast;

	char *raw_line;
	int n_pipes;
	int lt_ast_exit_status;
	int should_exit;
	int exit_code;

	/* state */
	char lexer_unclosed_quote;
	int lexer_need_more;

	char **envp;
	char **paths;
	int lineno;
} t_minishell;

void ms_clear(t_minishell *ms);

#endif
