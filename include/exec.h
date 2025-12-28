/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 19:03:49 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/28 04:53:30 by yzhang2          ###   ########.fr       */
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
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>
#include <string.h>

typedef struct s_fd_save
{
	int	in;
	int	out;
}		t_fd_save;

int		exec_ast(t_minishell *msh, ast *root);
int		exec_cmd_root(t_minishell *msh, ast *node);
int		exec_pipe_root(t_minishell *msh, ast *node);

int		exec_cmd_node(t_minishell *msh, ast *node, int in_fd, int out_fd);
int		exec_pipe_node(t_minishell *msh, ast *node, int in_fd, int out_fd);
int		pipe_collect(ast *root, ast ***out_arr, int *out_n);
void	child_exec_one(t_minishell *msh, ast *node, int in_fd, int out_fd);

int		apply_redir_list(t_redir *r, int *in_fd, int *out_fd);
int		dup_in_out_or_close(int in_fd, int out_fd);

int		exec_refresh_paths(t_minishell *msh);
int		ensure_paths_ready(t_minishell *msh);
void	free_paths(t_minishell *msh);

char	*find_cmd_path(t_minishell *msh, const char *cmd);

int		save_std_fds(t_fd_save *save);
void	restore_std_fds(t_fd_save *save);
void	set_status_from_wait(t_minishell *msh, int st);
int		wait_pair_set_right(t_minishell *msh, pid_t left, pid_t right);

int		is_builtin(const char *cmd);
int		exec_builtin(ast *node, t_env **env);
void	change_envp(t_env *env, char ***envp);
int		run_builtin_parent(t_minishell *msh, ast *node, int in_fd, int out_fd);

#endif
