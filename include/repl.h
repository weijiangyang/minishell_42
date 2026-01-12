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

typedef struct s_ast		t_ast;
typedef struct s_minishell	t_minishell;

void	repl_loop(t_minishell *ms);
char	*repl_read(char *acc);
int		repl_join(char **acc, char *line);
int		repl_has_text(const char *s);
void	repl_free_acc(char **acc);
void	repl_run_acc(t_minishell *ms, char **acc);
int		repl_step(t_minishell *ms, char **acc);

/* repl_utils.c */
int		lexer_has_heredoc(t_lexer *lx);
char	*dup_first_line(const char *s);

#endif