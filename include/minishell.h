/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 15:21:40 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 18:09:54 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <signal.h>

/* forward declarations */
typedef struct s_lexer t_lexer;
typedef struct s_env   t_env;

extern volatile sig_atomic_t g_signal;

typedef enum e_parse_status
{
    PARSE_OK,
    PARSE_SYNTAX_ERROR,
    PARSE_INCOMPLETE_INPUT   // 关键！
}   t_parse_status;
typedef struct s_minishell
{
	t_lexer	*lexer;
	t_env	*env;

	char	*raw_line;

	int		n_pipes;
	int		last_exit_status;

	/* state */
	char					lexer_unclosed_quote; /* '\0', '\'', '"' */
	int						lexer_need_more;

	char	**envp;
	char	**paths;
	int		lineno;
	char   *input_line;
	int		parse_status;
}	t_minishell;

#endif
