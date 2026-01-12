/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 12:25:18 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/12 12:25:29 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

# include "parse.h"
# include "../libft/libft.h"
# include "build_in.h"

enum	e_qstate
{
	Q_NONE = 0,
	Q_SQ = 1,
	Q_DQ = 2
};

typedef struct s_exp_data
{
	t_minishell		*minishell;
	char			**out;
}					t_exp_data;

int		word_has_quotes(const char *s);
int		expander_t_ast(t_minishell *minishell, t_ast *root);
int		expander_expand_cmd_node(t_minishell *msh, t_ast *node);
char	*expander_str(t_minishell *minishell, char *str);
int		scan_expand_one(t_exp_data *data, const char *s, int j,
			enum e_qstate q);
char	*expand_all(t_minishell *minishell, const char *str);
int		is_name_start(int c);
int		is_name_char(int c);
int		var_len(const char *s);
char	*env_value_dup(t_minishell *minishell, const char *name, int len);
char	*str_join_free(char *a, const char *b);
size_t	equal_sign(char *str);
char	*remove_quotes_flag(const char *s, int *had_q, int *q_s, int *q_d);
char	*expand_heredoc_vars(t_minishell *ms, char *line);

#endif
