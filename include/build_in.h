/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_in.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 18:05:49 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 18:11:47 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILD_IN_H
# define BUILD_IN_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>
#include <termios.h>

#include "parse.h"
#include "build_in.h"



typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}					t_env;

int					exec_builtin(ast *node, t_env **env);
int					is_builtin(const char *cmd);
int					ft_cd(char **argv, t_env **env);
int					ft_echo(char **argv);
int					builtin_env(char **argv, t_env *env);
void				print_env(t_env *env);
t_env				*env_new(char *key, char *value);
void				env_add_back(t_env **env, t_env *new_env);
t_env				*init_env(char **envp);
int					builtin_export(char **argv, t_env **env);
int					builtin_unset(char **argv, t_env **env);
t_env				*find_env_var(t_env *env, const char *key);
void				change_envp(t_env *env, char ***envp);
int					is_valid_identifier(const char *s);
void				free_env(t_env *env);
int					builtin_exit(char **argv);
int					builtin_pwd(void);

#endif
