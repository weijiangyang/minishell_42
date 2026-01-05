/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:12 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:17:18 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/build_in.h"

/**
 * @brief 以 Bash 标准格式打印所有环境变量。
 * * 行为逻辑：
 * 1. 遍历链表：从头到尾访问环境变量。
 * 2. 格式化输出：每个变量前缀固定为 "declare -x "。
 * 3. 处理空值：
 * - 如果变量有值 (value != NULL)，打印 `="value"`（注意：值被双引号包裹）。
 * - 如果变量仅被声明但未赋值 (value == NULL)，则只打印变量名。
 * 4. 换行：每个变量打印结束后输出换行符。
 * * @param env 环境变量链表的首节点。
 */
void print_export(t_env *env)
{
    while (env)
    {
        printf("declare -x %s", env->key);
        if (env->value)
            printf("=\"%s\"", env->value);
        printf("\n");
        env = env->next;
    }
}

/**
 * @brief 解析 export 命令的单个字符串参数。
 * * 解析逻辑：
 * 1. 查找分隔符：使用 strchr 搜索等号 '='。
 * 2. 带值的变量 (KEY=VALUE):
 * - 提取 KEY：等号前的所有字符。
 * - 提取 VALUE：等号之后的所有内容（包括空字符串 ""）。
 * 3. 仅声明变量 (KEY):
 * - 提取 KEY：整个参数字符串。
 * - 设置 VALUE 为 NULL，表示该变量已导出但尚未赋值。
 * 4. 内存管理：为解析出的字符串分配独立堆内存，失败时确保清理已分配部分。
 * * @param arg   输入的原始参数字符串。
 * @param key   [输出参数] 解析后的键名指针。
 * @param value [输出参数] 解析后的值指针（若无等号则为 NULL）。
 * @return int   成功返回 0，内存分配失败返回 -1。
 */
static int parse_export_arg(char *arg, char **key, char **value)
{
    char *equal;

    equal = strchr(arg, '=');
    if (equal)
    {
        *key = strndup(arg, equal - arg);
        *value = strdup(equal + 1);
        if (!*key || !*value)
        {
            perror("strdup");
            free(*key);
            free(*value);
            return -1;
        }
    }
    else
    {
        *key = strdup(arg);
        *value = NULL;
        if (!*key)
            return (perror("strdup"), -1);
    }
    return 0;
}

/**
 * @brief 更新已存在的环境变量或添加新变量（专门针对 export 逻辑）。
 * * 覆盖规则（模拟 Bash 行为）：
 * 1. 查找变量：检查 key 是否已存在于链表中。
 * 2. 变量已存在：
 * - 如果新 value 不为 NULL：说明输入是 `export KEY=VALUE`，此时执行覆盖操作，
 * 释放旧值并指向新值。
 * - 如果新 value 为 NULL：说明输入是 `export KEY`，Bash 规定此时不应修改旧值。
 * - 无论是否更新，都要释放传入的 key 内存（因为旧节点已持有同名的 key）。
 * 3. 变量不存在：
 * - 直接调用 env_new 和 env_add_back 创建并挂载新节点。
 * * @param env   环境变量链表的二级指针。
 * @param key   解析好的键名（必须是堆分配的内存）。
 * @param value 解析好的键值（可以是 NULL 或堆分配的内存）。
 */
static void update_env_var(t_env **env, char *key, char *value)
{
    t_env *existing;

    existing = find_env_var(*env, key);
    if (existing)
    {
        if (value)
        {
            free(existing->value);
            existing->value = value;
        }
        free(key);
    }
    else
        env_add_back(env, env_new(key, value));
}

/**
 * @brief 处理 export 命令中的单个参数项。
 * * 步骤如下：
 * 1. 解析参数：调用 parse_export_arg 将 "KEY=VALUE" 或 "KEY" 拆分。
 * 2. 合法性检查：调用 is_valid_identifier 验证键名是否符合 Shell 命名规则。
 * - 规则：必须以字母或下划线开头，后续字符只能是字母、数字或下划线。
 * 3. 错误处理：若键名非法，输出报错信息，释放临时内存并返回 1。
 * 4. 更新/插入：若合法，调用 update_env_var 处理变量的更新或链表挂载。
 * * @param arg  原始参数字符串（如 "VAR=123"）。
 * @param env  环境变量链表的二级指针。
 * @return int 成功返回 0，解析失败或键名非法返回 1。
 */
static int export_one(char *arg, t_env **env)
{
    char *key;
    char *value;

    if (parse_export_arg(arg, &key, &value) == -1)
        return 1;

    if (!is_valid_identifier(key))
    {
        fprintf(stderr, "export: `%s': not a valid identifier\n", arg);
        free(key);
        free(value);
        return 1;
    }

    update_env_var(env, key, value);
    return 0;
}

/**
 * @brief export 内置命令的主函数。
 * * 运行逻辑：
 * 1. 列表显示模式：若 argv 仅包含命令本身（无参数），调用 print_export 以 
 * "declare -x KEY="VALUE"" 格式打印所有变量。
 * 2. 变量设置模式：遍历从 argv[1] 开始的所有参数。
 * - 对每个参数调用 export_one 进行解析、校验和存储。
 * - 如果其中任何一个参数不合法（如 `export 123VAR`），将 status 设为 1，
 * 但继续处理剩余参数。
 * 3. 状态返回：返回 0（全部成功）或 1（至少有一个参数处理失败）。
 * * @param argv 命令及其参数数组。
 * @param env  环境变量链表的二级指针。
 * @return int 退出状态码。
 */
int builtin_export(char **argv, t_env **env)
{
    int status = 0;
    int i = 1;

    if (!argv[1])
    {
        print_export(*env);
        return 0;
    }
    while (argv[i])
    {
        if (export_one(argv[i], env))
            status = 1;
        i++;
    }
    return status;
}
