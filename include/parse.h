/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:21:13 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/12 11:16:48 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "../libft/libft.h"
# include "lexer.h"
# include "signals.h"
# include "error.h"

# define BUFFER_SIZE 42

typedef struct s_list	t_list;
typedef t_list			t_cmd;

typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_SUBSHELL,
}						t_node_type;

typedef enum e_redir_type
{
	REDIR_INPUT,
	REDIR_OUTPUT,
	REDIR_APPEND,
	HEREDOC
}						t_redir_type;

typedef struct s_redir
{
	struct s_redir		*next;
	char				*filename;
	int					heredoc_fd;
	int					is_expanded;
	t_redir_type		type;
	int					quoted;
}						t_redir;

typedef struct s_ast
{
	t_node_type			type;
	char				**argv;
	t_redir				*redir;
	int					n_pipes;
	struct s_ast		*left;
	struct s_ast		*right;
	struct s_ast		*sub;
}						t_ast;

/* free functions */
void		free_ast(t_ast *node);
void		free_tokens(t_lexer *tok);
void		free_ast_partial(t_ast *node);
void		free_argv_list(t_cmd *a);
void		free_redir_list(t_redir *r);
void		free_t_cmd_node(t_cmd *argv_cmd);

/* lexer utils */
t_lexer		*peek_token(t_lexer **cur);
t_lexer		*consume_token(t_lexer **cur);
t_lexer		*expect_token(t_tok_type type, t_lexer **cur);
int			is_redir_token(t_lexer *pt);
const char	*t_tok_type_to_str(t_tok_type type);

/* debug / print */
void		print_indent(int depth);
void		print_ast(t_ast *node, int depth);
void		print_ast_by_type(t_ast *node, int depth);
void		print_ast_pipe(t_ast *node, int depth);
void		print_ast_cmd(t_ast *node);
void		print_ast_subshell(t_ast *node, int depth);

/* parsing */
t_ast		*parse_cmdline(t_lexer **cur, t_minishell *minishell);
t_ast		*parse_pipeline(t_lexer **cur, t_minishell *minishell);
t_ast		*parse_subshell(t_lexer **cur, t_ast *node, t_minishell *minishell);
t_ast		*parse_simple_cmd_redir_list(t_lexer **cur, t_minishell *minishell);
t_ast		*parse_normal_cmd_redir_list(t_lexer **cur, t_ast *node,
				t_minishell *msh);

/* argv / redir */
t_cmd		*create_argv(char *str);
char		**build_argvs(t_cmd *argv_cmd, t_redir *redir, t_ast *node);

/* env / safe string utils */
char		*safe_strdup(const char *s);

/* redirection / heredoc */
int			handle_heredoc(t_redir *new_redir, t_minishell *minishell);
int			build_redir(t_lexer **cur, t_redir **redir_list, t_minishell *msh);
int			end_line(char *str);
char		*extract_line(char *str);
int			heredoc_loop(int write_fd, const char *delim, t_minishell *msh,
				int quoted);

#endif
