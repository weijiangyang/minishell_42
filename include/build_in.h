/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_in.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:25:53 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/11 18:44:04 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILD_IN_H
# define BUILD_IN_H

# include <limits.h>
# include <unistd.h>
# include "../libft/libft.h"
# include "error.h"
# include "parse.h"

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

int		exec_builtin(t_ast *node, t_env **env, t_minishell *msh);
int		is_builtin(const char *cmd);
int		ft_cd(char **argv, t_env **env, t_minishell *msh);
int		ft_echo(char **argv);
int		builtin_pwd(void);
int		builtin_env(char **argv, t_env *env);
int		builtin_export(char **argv, t_env **env);
int		builtin_unset(char **argv, t_env **env);
int		builtin_exit(char **argv, t_minishell *msh);
t_env	*init_env(char **envp);
t_env	*env_new(char *key, char *value);
void	env_add_back(t_env **env, t_env *new_env);
t_env	*find_env_var(t_env *env, const char *key);
void	env_set(t_env **env, const char *key, const char *value);
void	change_envp(t_env *env, char ***envp);
void	print_env(t_env *env);
void	free_env(t_env *env);
int		is_valid_identifier(const char *s);
void	free_char_matrix(char **matrix);
void	print_export(t_env *env);

#endif
