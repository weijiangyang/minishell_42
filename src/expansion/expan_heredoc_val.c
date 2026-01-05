/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_heredoc_val.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:20:22 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:20:24 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"
#include "../../include/build_in.h"

/**
 * @brief 在环境变量链表中根据键名 (key) 查找对应的值 (value)。
 * * 处理逻辑：
 * 1. 遍历链表：从 ms->env 头节点开始，依次对比每个节点的 key。
 * 2. 匹配成功：使用 ft_strcmp 进行精确匹配，若相同则返回该节点的 value 指针。
 * 3. 匹配失败：如果遍历完整个链表仍未找到，返回空字符串 ""。
 * * 注意：返回空字符串 "" 而非 NULL 是为了方便调用方（如 ft_strjoin）直接拼接，
 * 避免了额外的空指针判断，符合 Shell 对未定义变量的处理逻辑。
 * * @param ms  全局上下文结构体，包含环境变量链表。
 * @param key 需要查找的环境变量名（如 "HOME"）。
 * @return char* 返回对应的值指针；若不存在则返回 ""。
 */
char *get_env_value(t_minishell *ms, const char *key)
{
    t_env *env = ms->env;
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
            return env->value;
        env = env->next;
    }
    return "";
}

/**
 * @brief 处理 $? 展开：将最后一次执行的状态码转换为字符串并拼接。
 * * 1. 数字化转字符串：使用 ft_printf（或 snprintf 变体）将 ms->last_exit_status
 * （通常是 0-255 之间的整数）写入临时缓冲区 tmp。
 * 2. 内存拼接：调用 ft_strjoin_free 将转换后的状态码字符串连接到已有的结果 res 上。
 * 3. 自动释放：依靠 ft_strjoin_free 释放旧的 res 指针，防止在循环拼接中产生内存泄漏。
 * * @param ms  全局上下文，包含最近一次命令的退出状态 last_exit_status。
 * @param res 当前已构建的展开结果字符串。
 * @return char* 返回拼接了状态码后的新字符串指针。
 */
static char *expand_exit_status(t_minishell *ms, char *res)
{
    char tmp[32];

    ft_printf(tmp, sizeof(tmp), "%d", ms->last_exit_status);
    return ft_strjoin_free(res, tmp);
}

/**
 * @brief 解析并展开字符串中的环境变量（$VAR）。
 * * 处理逻辑：
 * 1. 确定长度：从当前位置开始，扫描符合变量命名规则（字母、数字、下划线）的字符长度。
 * 2. 提取键名：使用 ft_substr 截取变量名（Key）。
 * 3. 获取键值：调用 get_env_value 从环境链表中获取对应的 Value。
 * 4. 字符串合并：将获取到的值通过 ft_strjoin_free 拼接到结果字符串 res 中。
 * 5. 指针偏移：通过 *adv 返回已处理的变量名长度，以便主循环跳过这些字符。
 * * @param ms  全局上下文，包含环境变量链表。
 * @param ptr 指向字符串中紧跟在 '$' 符号后的起始位置。
 * @param res 当前已经构建好的展开结果字符串。
 * @param adv [输出参数] 用于记录被消耗的变量名长度。
 * @return char* 返回拼接了变量值后的新结果字符串。
 */
static char *expand_env_var(t_minishell *ms, char *ptr, char *res, int *adv)
{
    char *key;
    char *val;
    int len;

    len = 0;
    while (ft_isalnum(ptr[len]) || ptr[len] == '_')
        len++;
    key = ft_substr(ptr, 0, len);
    if (!key)
        return res;
    val = get_env_value(ms, key);
    res = ft_strjoin_free(res, val ? val : "");
    free(key);
    *adv = len;
    return res;
}

/**
 * @brief 将单个字符追加到动态增长的结果字符串末尾。
 * * 1. 构造字符串：将字符 c 放入一个长度为 2 的字符数组 tmp 中，并补齐 '\0'。
 * 2. 内存拼接：调用 ft_strjoin_free 将该临时字符串连接到 res。
 * 3. 自动回收：ft_strjoin_free 会负责释放旧的 res 指针并返回分配的新指针。
 * * @param res 当前已构建的结果字符串。
 * @param c   待追加的单个字符。
 * @return char* 返回追加字符后的新字符串指针。
 */
static char *append_char(char *res, char c)
{
    char tmp[2];

    tmp[0] = c;
    tmp[1] = '\0';
    return ft_strjoin_free(res, tmp);
}

/**
 * @brief Heredoc 变量展开的主调度函数。
 * * 该函数采用分治策略，将复杂的展开逻辑委托给专项辅助函数：
 * 1. 初始化：创建一个空字符串作为累加基底。
 * 2. 状态机扫描：
 * - 遇到 "$?" : 调用 expand_exit_status 处理状态码。
 * - 遇到 "$VAR" : 调用 expand_env_var 处理环境变量，并根据变量名长度移动指针。
 * - 遇到 孤立"$" : 直接追加字符 '$'。
 * - 遇到 普通字符 : 调用 append_char 逐个追加。
 * 3. 内存管理：每个辅助函数都通过 ft_strjoin_free 维护结果字符串 res 的内存。
 * * @param ms   全局上下文。
 * @param line 原始输入行。
 * @return char* 展开后的完整字符串。
 */
char *expand_heredoc_vars(t_minishell *ms, char *line)
{
    char *res;
    int adv;

    res = ft_strdup("");
    while (*line)
    {
        if (*line == '$')
        {
            line++;
            if (*line == '?')
                res = expand_exit_status(ms, res);
            else if (ft_isalpha(*line) || *line == '_')
            {
                adv = 0;
                res = expand_env_var(ms, line, res, &adv);
                line += adv - 1;
            }
            else
                res = ft_strjoin_free(res, "$");
        }
        else
            res = append_char(res, *line);
        line++;
    }
    return res;
}
