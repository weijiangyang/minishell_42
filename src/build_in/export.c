/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:12 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/12 11:43:47 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

/*
** 将 export 的参数解析为 key 和 value。
** 1. 寻找 '=' 字符。
** 2. 如果存在 '='，分割为 key 和 value（value 为空字符串 "" 是合法的）。
** 3. 如果不存在 '='，只有 key，value 设为 NULL。
*/
static int	parse_export_arg(char *arg, char **key, char **value)
{
	char	*equal;

	equal = ft_strchr(arg, '=');
	if (equal)
	{
		*key = ft_substr(arg, 0, equal - arg);
		*value = ft_strdup(equal + 1);
		if (!*key || !*value)
		{
			free(*key);
			free(*value);
			return (-1);
		}
	}
	else
	{
		*key = ft_strdup(arg);
		*value = NULL;
		if (!*key)
			return (-1);
	}
	return (0);
}

/*
** 更新或增加环境变量。
** 1. 查找是否存在同名的 key。
** 2. 如果存在且有新 value，释放旧值并更新。若新值为 NULL（如 export VAR），则保留原样。
** 3. 如果不存在，则在链表末尾新建节点。
*/
static void	update_env_var(t_env **env, char *key, char *value)
{
	t_env	*existing;

	existing = find_env_var(*env, key);
	if (existing)
	{
		if (value)
		{
			free(existing->value);
			existing->value = value;
		}
		// 既然已找到存在的变量，原本解析出的 key 就没用了，需要释放
		free(key);
	}
	else
		env_add_back(env, env_new(key, value));
}

/*
** 处理单个 export 参数（例如 "VAR=VAL" 或 "VAR"）。
** 1. 解析参数。
** 2. 校验 key 是否为合法的标识符（必须以字母或下划线开头，仅包含字母数字下划线）。
** 3. 合法则更新，不合法则报错。
*/
static int	export_one(char *arg, t_env **env)
{
	char	*key;
	char	*value;

	if (parse_export_arg(arg, &key, &value) == -1)
		return (1);
	if (!is_valid_identifier(key))
	{
		ms_put3("minishell: export: `", key, "': not a valid identifier\n");
		free(key);
		free(value);
		return (1);
	}
	update_env_var(env, key, value);
	return (0);
}

/*
** export 内建函数入口。
** 1. 如果没有参数，按字母顺序打印所有变量（即你之前写的 print_export）。
** 2. 循环处理所有参数，若其中任何一个失败，最终返回 1。
*/
int	builtin_export(char **argv, t_env **env)
{
	int	status;
	int	i;

	status = 0;
	i = 1;
	if (!argv[1])
	{
		print_export(*env);
		return (0);
	}
	while (argv[i])
	{
		if (export_one(argv[i], env))
			status = 1;
		i++;
	}
	return (status);
}