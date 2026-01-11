/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_env_chars.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 14:43:59 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 14:44:02 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "build_in.h"

/**
 * @brief 释放整个环境变量链表并回收所有相关内存。
 * * 内存管理策略：
 * 1. 迭代销毁：使用 tmp 指针暂存当前节点，防止在释放后丢失链表后续引用。
 * 2. 深度释放：
 * - 释放 key 字符串。
 * - 释放 value 字符串（即使是 NULL，free 也是安全的）。
 * - 释放 t_env 结构体节点本身。
 * 3. 链表前进：将 env 指针指向下一个节点，循环直至链表末尾。
 * * @param env 环境变量链表的头指针。
 */
void free_env(t_env *env)
{
    t_env *tmp;
    while (env)
    {
        tmp = env;
        env = env->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

/**
 * @brief 释放以 NULL 结尾的字符串数组（矩阵）及其包含的所有字符串。
 * * 内存结构回收逻辑：
 * 1. 空指针保护：如果传入的矩阵本身为 NULL，直接返回以避免段错误。
 * 2. 深度释放：
 * - 遍历数组：由于数组必须以 NULL 结尾（这是 C 语言处理这类结构的规范），
 * 循环释放每一个元素 `matrix[i]`。
 * - 归还字符串内存：释放每个子字符串占用的堆空间。
 * 3. 最终销毁：释放承载这些指针的顶层容器 `matrix` 本身。
 * * @param matrix 指向待释放字符串数组的指针。
 */
void free_char_matrix(char **matrix)
{
    int i = 0;
    if (!matrix)
        return ;
    while (matrix[i])
    {
        free(matrix[i]);
        i++;
    }
    free(matrix);
}
