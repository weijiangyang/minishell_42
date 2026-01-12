/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 15:40:59 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/12 10:50:39 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include "../libft/libft.h"
# include <ctype.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

# define LEX_OK 1
# define LEX_ERR 0
# define LEX_NEED_MORE 2
# define LEX_UNCLOSED_QUOTE -2

typedef struct s_minishell	t_minishell;

typedef struct s_token_info
{
	char					*clean;
	char					*raw;
	int						had_quotes;
	int						quoted_single;
	int						quoted_double;
}							t_token_info;

typedef enum e_tok_type
{
	TOK_WORD,
	TOK_PIPE,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_APPEND,
	TOK_HEREDOC,
	TOK_END,
	TOK_ERROR
}							t_tok_type;

typedef struct s_lexer
{
	char					*str;
	t_tok_type				tokentype;
	int						idx;
	int						had_quotes;
	int						quoted_by;
	char					*raw;
	struct s_lexer			*prev;
	struct s_lexer			*next;
}							t_lexer;

typedef struct s_index
{
	size_t					i;
	size_t					j;
}							t_index;

const char	*token_symbol(t_tok_type t);
t_lexer		*new_node(t_token_info *info, t_tok_type tokentype);
void		list_add_back(t_lexer **lst, t_lexer *new);
int			add_node(t_token_info *info, t_tok_type tokentype, t_lexer **list);
t_lexer		*clear_one(t_lexer **lst);
void		del_first(t_lexer **lst);
void		del_one(t_lexer **lst, int target);
void		clear_list(t_lexer **lst);
int			detect_signal(t_lexer **list);
t_tok_type	is_token(int c);
int			finalize_word_node(t_token_info *info, t_lexer **list);
int			handle_token(char *str, int idx, t_lexer **list);
int			match_quotes(int i, char *str, char quote);
char		*remove_quotes_flag(const char *s, int *had_q, int *q_s, int *q_d);
int			handle_word(char *str, int i, t_lexer **list, char *out_unclosed);
int			skip_spaces(char *str, int i);
int			handle_lexer(t_minishell *minishell);
int			check_space(char c);

#endif
