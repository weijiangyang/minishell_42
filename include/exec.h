/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:03:49 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/12 12:21:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# include "../libft/libft.h"
# include "build_in.h"
# include "error.h"
# include "parse.h"
# include "signals.h"
# include <errno.h>
# include <fcntl.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

typedef struct s_fd_save
{
	int				in;
	int				out;
}					t_fd_save;

typedef struct s_pipe_ctx
{
	t_minishell		*msh;
	t_ast			**arr;
	pid_t			*pids;
	t_ast			*root;
	int				n;
	int				in_fd;
	int				out_fd;
}					t_pipe_ctx;
typedef struct s_exec_ctx
{
	t_minishell		*msh;
	t_ast			*node;
	t_ast			*root;
	int				in_fd;
	int				out_fd;
}					t_exec_ctx;

int					exec_t_ast(t_minishell *msh, t_ast *root);
int					exec_cmd_root(t_minishell *msh, t_ast *node);
int					exec_pipe_root(t_minishell *msh, t_ast *node);
int					exec_cmd_node(t_minishell *msh, t_ast *node, int in_fd,
						int out_fd);
int					exec_pipe_node(t_minishell *msh, t_ast *node, int in_fd,
						int out_fd);
int					pipe_collect(t_ast *root, t_ast ***out_arr, int *out_n);
void				child_exec_one(t_exec_ctx *ctx);
int					apply_redir_list(t_redir *r, int *in_fd, int *out_fd);
int					dup_in_out_or_close(int in_fd, int out_fd);
int					exec_refresh_paths(t_minishell *msh);
int					ensure_paths_ready(t_minishell *msh);
void				free_paths(t_minishell *msh);
void				ms_child_exit(t_minishell *msh, t_ast *root, int status);
char				*find_cmd_path(t_minishell *msh, const char *cmd);
int					save_std_fds(t_fd_save *save);
void				restore_std_fds(t_fd_save *save);
void				set_status_from_wait(t_minishell *msh, int st);
int					wait_pair_set_right(t_minishell *msh, pid_t left,
						pid_t right);
int					prepare_heredocs(t_ast *node, t_minishell *ms);
int					run_builtin_parent_logic(t_minishell *msh, t_ast *node,
						int in_fd, int out_fd);

/* exec_child_utils.c */
int					path_is_dir(const char *path);
int					has_bad_heredoc(t_redir *r);
void				close_all_heredoc_fds(t_ast *node);

/* exec_pipe_utils.c */
void				wait_all_and_set_lt_ast(t_minishell *msh, pid_t *pids,
						int n);
int					pipe_make(int i, int n, int out_fd, int pfd[2]);
void				handle_parent_pipes(t_pipe_ctx *ctx, int pfd[2]);

#endif
