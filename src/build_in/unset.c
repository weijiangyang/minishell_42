/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:35 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:17:37 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/build_in.h"
#include "../../libft/libft.h"

/**
 * @brief 从环境变量链表中查找并彻底删除指定的变量。
 * * 算法逻辑：
 * 1. 空链表检查：若链表为空，直接返回。
 * 2. 头节点特殊处理：若待删变量正是头节点，需更新头指针 (*env) 指向第二个节点。
 * 3. 遍历搜索：使用两个指针（temp 指向当前，prev 指向前驱）同步移动。
 * 4. 节点断开：找到目标后，将 prev->next 指向 temp->next，从而在逻辑上跳过目标节点。
 * 5. 内存释放：依次释放 key、value 以及节点结构体本身，防止内存泄漏。
 * * @param env 指向环境变量链表头指针的二级指针（涉及修改头指针，必须传址）。
 * @param key 待删除的变量名。
 */
static void delete_env_var(t_env **env, const char *key)
{
    t_env *temp = *env;
    t_env *prev = NULL;

    if (!temp)
        return ;
    if (strcmp(temp->key, key) == 0)
    {
        *env = temp->next; 
        free(temp->key);
        free(temp->value);
        free(temp);
        return ;
    }
    while (temp != NULL && strcmp(temp->key, key) != 0)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
        return ;
    prev->next = temp->next;
    free(temp->key);
    free(temp->value);
    free(temp);
}

/**
 * @brief 验证字符串是否符合 Shell 变量名的命名规范。
 * * 命名规则（POSIX 标准）：
 * 1. 非空限制：字符串不能为空且不能为空字符串。
 * 2. 首字符规则：必须以字母 (a-z, A-Z) 或下划线 (_) 开头，不能以数字开头。
 * 3. 后续字符规则：只能包含字母、数字 (0-9) 或下划线 (_)。
 * * @param s 待校验的变量名字符串。
 * @return int 合法返回 1，非法返回 0。
 */
int is_valid_identifier(const char *s)
{
    int i = 0;

    if (!s || !s[0])
        return 0;

    if (!(ft_isalpha(s[0]) || s[0] == '_'))
        return 0;

    i = 1;
    while (s[i])
    {
        if (!(ft_isalnum(s[i]) || s[i] == '_'))
            return 0;
        i++;
    }
    return 1;
}

/**
 * @brief unset 内置命令的主函数。
 * * 运行逻辑：
 * 1. 遍历参数：从 argv[0] 开始（注：应从 argv[1] 开始，跳过 "unset" 自身）。
 * 2. 合法性检查：调用 is_valid_identifier 验证待删除的变量名。
 * - 若非法：向 stderr 报错并标记 status 为 1，但不会停止循环。
 * 3. 执行删除：若变量名合法，调用 delete_env_var 从链表中移除该节点。
 * - 如果变量本身不存在，delete_env_var 会静默处理，这符合 Shell 规范。
 * 4. 状态返回：若所有参数合法返回 0，只要有一个非法则返回 1。
 * * @param argv 命令及参数数组。
 * @param env  环境变量链表的二级指针。
 * @return int 退出状态码。
 */
int builtin_unset(char **argv, t_env **env)
{
    int status;
    int i;

    i = 0;
    status = 0;
    while (argv[i])
    {
        if (!is_valid_identifier(argv[i]))
        {
            fprintf(stderr,
                "unset: `%s': not a valid identifier\n",
                argv[i]);
            status = 1;
        }
        else
            delete_env_var(env, argv[i]);
        i++;
    }
    return status;
}
