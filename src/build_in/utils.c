/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:56 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:17:57 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

/**
 * @brief 计算环境变量链表中的节点总数。
 * * 1. 迭代计数：从头节点开始遍历整个单链表。
 * 2. 终止条件：当指针到达链表末尾（NULL）时停止。
 * 3. 核心用途：
 * - 为 `execve` 所需的字符串数组分配精确的内存大小 (len + 1)。
 * - 在执行不带参数的 `export` 时，用于确定排序所需的临时空间。
 * * @param env 环境变量链表的头指针。
 * @return int 链表中的节点个数。
 */
static int env_len(t_env *env)
{
    int len = 0;

    while (env)
    {
        len++;
        env = env->next;
    }
    return len;
}

/**
 * @brief 将环境变量节点转换为 "KEY=VALUE" 格式的字符串。
 * * 拼接逻辑：
 * 1. 键名处理：首先将变量名 (key) 与等号 ("=") 拼接。
 * 2. 最终合成：在上述结果基础上，拼接变量值 (value)。
 * 3. 内存管理：
 * - 拼接过程中产生的中间字符串 (key_value) 会被及时释放。
 * - 返回的 result 是一个新分配的堆内存字符串，调用方负责后续释放。
 * * @param env 指向单个环境变量节点的指针。
 * @return char* 拼接后的字符串（如 "PATH=/usr/bin"），分配失败则返回 NULL。
 */
static char *env_to_str(t_env *env)
{
    char *key_value;
    char *result;

    key_value = ft_strjoin(env->key, "=");
    if (!key_value)
        return NULL;
    result = ft_strjoin(key_value, env->value);
    free(key_value);
    return result;
}

/**
 * @brief 更新外部环境矩阵（envp），同步链表中的最新变量。
 * * 运行逻辑：
 * 1. 内存清理：如果传入的 `*envp` 矩阵非空，先调用 free_char_matrix 彻底释放旧内存。
 * 2. 空间分配：调用 env_len 获取链表长度，并为新矩阵分配 (len + 1) 个指针空间。
 * 3. 数据转换：遍历链表，调用 env_to_str 将每个节点转为 "KEY=VALUE" 字符串并填入矩阵。
 * 4. 结尾封口：在矩阵末尾填入 NULL 指针，符合 C 语言字符串数组的标准规范。
 * * @param env  指向当前环境变量链表的头指针。
 * @param envp 指向外部环境变量矩阵指针的二级指针（用于直接修改该指针指向的地址）。
 */
void change_envp(t_env *env, char ***envp)
{
    int i;
    t_env *tmp;
    int len;

    if (*envp != NULL)
    {
        free_char_matrix(*envp);
        *envp = NULL;
    }
    len = env_len(env);
    *envp = malloc(sizeof(char *) * (len + 1));
    if (!*envp)
        return ;
    tmp = env;
    i = 0;
    while (tmp)
    {
        (*envp)[i] = env_to_str(tmp);
        tmp = tmp->next;
        i++;
    }
    (*envp)[i] = NULL;
}

/**
 * @brief 遍历并打印环境变量链表（模拟 env 命令行为）。
 * * 打印逻辑：
 * 1. 条件过滤：只有当 `env->value` 不为 NULL 时才执行打印。
 * - 这是区分 `env` 和 `export` 命令的关键：`env` 忽略未赋值变量。
 * 2. 格式规范：按照标准的 `KEY=VALUE` 格式输出，每行一个变量。
 * 3. 链表迭代：逐个节点移动，直到到达链表末尾。
 * * @param env 环境变量链表的头指针。
 */
void print_env(t_env *env)
{
    while (env)
    {
        if (env->value) // n'afficher que KEY=VALUE
        {
            printf("%s=%s\n", env->key, env->value);
        }
        env = env->next;
    }
}
/**
 * @brief 在环境变量链表中根据键名 (key) 查找特定节点。
 * * 查找逻辑：
 * 1. 线性搜索：从头节点开始，沿着 next 指针遍历链表。
 * 2. 匹配检查：使用 strcmp 比较当前节点的 key 与目标 key。
 * 3. 结果返回：
 * - 匹配成功：返回该节点的指针，以便读取或修改其 value。
 * - 匹配失败：如果遍历到链表末尾 (NULL) 仍未找到，返回 NULL。
 * * @param env 环境变量链表的头指针。
 * @param key 要查找的变量名字符串。
 * @return t_env* 指向目标节点的指针，未找到则返回 NULL。
 */

t_env *find_env_var(t_env *env, const char *key)
{
    while (env)
    {
        if (strcmp(env->key, key) == 0)
            return env;
        env = env->next;
    }
    return NULL;
}