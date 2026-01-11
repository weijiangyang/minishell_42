/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:16:46 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:16:49 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

/**
 * @brief 创建一个新的环境变量链表节点（直接赋值模式）。
 * * 特点：
 * 1. 浅拷贝：直接将传入的 key 和 value 指针赋值给新节点。
 * 2. 内存所有权：调用方必须确保传入的字符串是在堆上分配的，
 * 并且在节点销毁前不被释放。
 * 3. 容错处理：若 malloc 失败则返回 NULL。
 * * @param key   已分配好的变量名字符串指针。
 * @param value 已分配好的变量值字符串指针。
 * @return t_env* 指向新节点的指针，失败则返回 NULL。
 */
t_env	*env_new(char *key, char *value)
{
	t_env	*new_env;

	new_env = (t_env *)malloc(sizeof(t_env));
	if (!new_env)
		return (NULL);
	new_env->key = key;
    new_env->value = value;
	new_env->next = NULL;
	return (new_env);
}
/**
 * @brief 将创建好的新环境变量节点安全地添加到链表末尾。
 * * 1. 鲁棒性检查：验证 env 指针地址及 new_env 节点是否有效。
 * 2. 头节点初始化：若链表为空（*env == NULL），将新节点设为起始节点。
 * 3. 线性寻址：遍历链表直至找到最后一个节点（next 指针为 NULL）。
 * 4. 链接：将最后一个节点的 next 指向新节点，完成挂载。
 * * @param env     指向环境变量链表头指针的二级指针。
 * @param new_env 待插入的新节点指针。
 */

void	env_add_back(t_env **env, t_env *new_env)
{
	t_env	*last;

	if (!env || !new_env)
		return ;
	if (*env == NULL)
	{
		*env = new_env;
		return ;
	}
	last = *env;
	while (last->next)
		last = last->next;
	last->next = new_env;
}

/**
 * @brief 将系统环境变量数组转换为内部链表结构。
 * * 执行流程：
 * 1. 迭代数组：遍历 `char **envp`（格式通常为 "KEY=VALUE"）。
 * 2. 定位分隔符：使用 strchr 找到第一个 '=' 的位置。
 * 3. 拆分键值对：
 * - key: 截取 '=' 之前的内容（使用 strndup）。
 * - value: 拷贝 '=' 之后的内容（使用 strdup）。
 * 4. 封装与挂载：调用 env_new 创建节点，并用 env_add_back 存入链表。
 * 5. 状态返回：返回构建好的链表头指针。
 * * @param envp 系统传入的原始环境变量数组（main 函数的第三个参数）。
 * @return t_env* 指向初始化后的环境变量链表首节点的指针。
 */
t_env *init_env(char **envp)
{
    t_env *env;

    env = NULL;
    int     i = 0;

    while (envp[i])
    {
        char *equal = strchr(envp[i], '=');
        if (equal)
        {
            char *key = strndup(envp[i], equal - envp[i]);
            char *value = strdup(equal + 1);
            env_add_back(&env, env_new(key, value));
        }
        i++;
    }
    return env;
}

/**
 * @brief env 内置命令的入口。
 * * 行为逻辑：
 * 1. 参数检查：根据 Minishell 的常见要求，env 通常不处理任何选项或参数。
 * - 如果接收到参数（argv[1] 存在），由于不支持执行外部命令，
 * 会模仿报错并返回 127。
 * 2. 遍历输出：调用 print_env 循环打印链表中的每一个环境变量。
 * 3. 状态返回：成功打印后返回 0。
 * * @param argv 命令参数数组。
 * @param env  指向环境变量链表的首节点。
 * @return int 成功返回 0，带参数报错则返回 127。
 */
int builtin_env(char **argv, t_env *env)
{ 
    if (argv[1]) 
    {
        ms_put3("env: ", argv[1], " No such file or directory\n");
        return (127); 
    }
    print_env(env);
    return 0;
}

