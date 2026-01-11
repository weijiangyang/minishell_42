/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_set.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 14:01:46 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 14:01:49 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h"

/**
 * @brief 创建并初始化一个新的环境变量链表节点。
 * * 执行流程：
 * 1. 内存拷贝：使用 strdup 为键 (key) 和值 (value) 申请独立的堆内存。
 * 这样做是为了确保即使原始参数被修改或释放，环境变量依然有效。
 * 2. 节点分配：为 t_env 结构体分配内存。
 * 3. 容错处理：如果任何内存分配失败，立即释放已申请的内存并安全退出进程，
 * 防止产生脏数据或段错误。
 * 4. 成员初始化：将拷贝后的字符串关联到节点，并将 next 指针置为 NULL。
 * * @param key   环境变量名（如 "USER"）。
 * @param value 环境变量值（如 "root"）。
 * @return t_env* 指向新创建节点的指针。
 */
static t_env *create_env_var(const char *key, const char *value)
{
	t_env *new_var;
	char *new_key;
	char *new_value;

	new_key = strdup(key);
	new_value = strdup(value);
	if (!new_key || !new_value)
	{
		perror("strdup");
		free(new_key);
		free(new_value);
		exit(EXIT_FAILURE);
	}
	new_var = malloc(sizeof(t_env));
	if (!new_var)
	{
		perror("malloc");
		free(new_key);
		free(new_value);
		exit(EXIT_FAILURE);
	}
	new_var->key = new_key;
	new_var->value = new_value;
	new_var->next = NULL;
	return new_var;
}

/**
 * @brief 将新的环境变量节点追加到链表的末尾。
 * * 1. 空链表处理：如果当前环境变量链表为空（*env 为 NULL），
 * 直接将新节点设为头节点。
 * 2. 链表遍历：从头节点开始，一直移动到最后一个节点（next 为 NULL 的节点）。
 * 3. 挂载：将最后一个节点的 next 指针指向新节点。
 * * 这样做保证了环境变量在 `env` 命令输出时，通常保持着它们被定义或导入时的原始顺序。
 * * @param env      指向环境变量链表头指针的二级指针（以便修改头指针本身）。
 * @param new_var  待加入链表的新节点指针。
 */
static void add_env_var(t_env **env, t_env *new_var)
{
	t_env *lt_ast;

	if (!*env)
	{
		*env = new_var;
		return;
	}
	lt_ast = *env;
	while (lt_ast->next)
		lt_ast = lt_ast->next;
	lt_ast->next = new_var;
}

/**
 * @brief 设置环境变量的值（更新或新增）。
 * * 算法逻辑：
 * 1. 查找是否存在：调用 find_env_var 检查链表中是否已有名为 key 的变量。
 * 2. 更新逻辑（若存在）：
 * - 释放旧值的内存，防止内存泄漏。
 * - 使用 strdup 拷贝新值并赋值。
 * - 若分配失败，打印错误并退出（保持严谨的内存安全策略）。
 * 3. 新增逻辑（若不存在）：
 * - 调用 create_env_var 创建完整的新节点。
 * - 调用 add_env_var 将新节点挂载到链表末尾。
 * * @param env   指向环境变量链表头指针的二级指针。
 * @param key   变量名。
 * @param value 变量值。
 */
void env_set(t_env **env, const char *key, const char *value)
{
	t_env *var;

	var = find_env_var(*env, key);
	if (var)
	{
		free(var->value);
		var->value = strdup(value);
		if (!var->value)
		{
			perror("strdup");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		add_env_var(env, create_env_var(key, value));
	}
}
