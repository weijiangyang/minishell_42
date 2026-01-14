#include "minishell.h"
#include "build_in.h"

/*
** 计算环境链表中的节点总数。
** 返回节点数量。
*/
static int	env_size(t_env *env)
{
	int	i;

	i = 0;
	while (env)
	{
		i++;
		env = env->next;
	}
	return (i);
}

/*
** 将环境链表转换为以 NULL 结尾的指针数组。
** 这样做可以在不破坏原始链表结构的前提下，方便地对环境变量进行排序。
*/
static t_env	**env_to_array(t_env *env)
{
	t_env	**arr;
	int		i;
	int		size;

	size = env_size(env);
	arr = malloc(sizeof(t_env *) * (size + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (env)
	{
		arr[i++] = env;
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}

/*
** 使用冒泡排序算法，按键（key）的字母顺序对环境指针数组进行排序。
*/
static void	sort_env_array(t_env **arr)
{
	int		i;
	int		j;
	t_env	*tmp;

	i = 0;
	while (arr[i])
	{
		j = i + 1;
		while (arr[j])
		{
			if (ft_strcmp(arr[i]->key, arr[j]->key) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

/*
** 以字母顺序打印环境变量，符合 bash 的 export 命令格式：
** 1. 有值的变量：declare -x KEY="VALUE"
** 2. 无值的变量：declare -x KEY
*/
void	print_export(t_env *env)
{
	t_env	**arr;
	int		i;

	arr = env_to_array(env);
	if (!arr)
		return ;
	sort_env_array(arr);
	i = 0;
	while (arr[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(arr[i]->key, STDOUT_FILENO);
		if (arr[i]->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(arr[i]->value, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
		i++;
	}
	free(arr);
}