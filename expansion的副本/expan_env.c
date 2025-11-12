/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:34:06 by yzhang2           #+#    #+#             */
/*   Updated: 2025/11/12 19:09:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "expander.h"
#include "minishell.h"

// 做什么：判定是否是变量名首字符（字母或 _）。
// 谁调：var_len。
int	is_name_start(int c)
{
	return (ft_isalpha(c) || c == '_');
}

// 做什么：判定是否是变量名后续字符（字母/数字/_）。
// 谁调：var_len。
int	is_name_char(int c)
{
	return (ft_isalnum(c) || c == '_');
}

// 做什么：从 s 开始（即 $ 后第一个字符），算出合法变量名的长度。
// 谁调：handle_var_exp → scan_expand_one。
int	var_len(const char *s)
{
	int	i;

	i = 0;
	if (!is_name_start((unsigned char)s[i]))
		return (0);
	while (s[i] && is_name_char((unsigned char)s[i]))
		i++;
	return (i);
}

// 做什么：在 minishell->envp 中找 name[0..len-1] 的环境变量，返回值的 ft_strdup；找不到返回 ft_strdup("")。
// 实现细节：用 equal_sign(entry) 找 = 的位置，兼容不同返回语义；比较 key 后，值从 keylen+1（若 entry[keylen]=='='）或 keylen 开始。
// 谁调：handle_var_exp → scan_expand_one。
char	*env_value_dup(t_minishell *minishell, const char *name, int len)
{
	int		k;
	int		keylen;
	char	*entry;

	if (!minishell || !minishell->envp)
		return (ft_strdup(""));
	k = 0;
	while (minishell->envp[k])
	{
		entry = minishell->envp[k];
		keylen = equal_sign(entry);
		if (keylen == len && ft_strncmp(name, entry, len) == 0)
		{
			if (entry[keylen] == '=')
				return (ft_strdup(entry + keylen + 1));
			return (ft_strdup(entry + keylen));
		}
		k++;
	}
	return (ft_strdup(""));
}
