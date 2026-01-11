/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 15:21:40 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 02:54:13 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <signal.h>

/* forward declarations */
typedef struct s_lexer			t_lexer;
typedef struct s_env			t_env;
typedef struct s_ast			ast;

extern volatile sig_atomic_t	g_signal;

typedef struct s_minishell
{
	t_lexer						*lexer;
	t_env						*env;
	ast							*cur_ast;
	

	char						*raw_line;
	int							n_pipes;
	int							last_exit_status;
	int							should_exit;
	int							exit_code;

	/* state */
	char						lexer_unclosed_quote;
	int							lexer_need_more;


	char	**envp;
	char	**paths;
	int		lineno;
}	t_minishell;

void							ms_clear(t_minishell *ms);

#endif
