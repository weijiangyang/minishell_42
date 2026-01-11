/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_heredoc_val.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:20:22 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/09 00:31:37 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "expander.h"
#include "minishell.h"

/* 这个函数像是在书架上找书：给它一个书名（环境变量名），它就帮你把书里的内容（变量值）拿出来。
   如果没找到，就返回一个空的东西。 */
char    *search_env(t_minishell *ms, const char *key)
{
    t_env   *node;

    node = ms->env;
    while (node)
    {
        if (ft_strcmp(node->key, key) == 0)
            return (node->value);
        node = node->next;
    }
    return ("");
}

/* 这个函数处理那个特殊的问号变量 $?。它把上一次程序运行的结果数字变成文字，
   然后贴在我们要的结果后面。 */
static char *handle_status(t_minishell *ms, char *result, int *step)
{
    char    *value;

    value = ft_itoa(ms->last_exit_status);
    if (!value)
        return (result);
    result = ft_strjoin_free(result, value);
    free(value);
    *step = 1;
    return (result);
}

/* 这个函数处理普通的美元符号变量（比如 $HOME）。它先数出名字有多长，
   然后去前面的“书架”找内容，最后把找出来的东西接到结果后面。 */
static char *handle_variable(t_minishell *ms, char *target, char *result,
        int *step)
{
    char    *key;
    char    *value;
    int     length;

    length = 0;
    while (ft_isalnum(target[length]) || target[length] == '_')
        length++;
    key = ft_substr(target, 0, length);
    if (!key)
        return (result);
    value = search_env(ms, key);
    result = ft_strjoin_free(result, value);
    free(key);
    *step = length;
    return (result);
}

/* 这个函数很简单，就是把一个落单的字母或符号，紧紧地粘在现有的文字末尾。 */
static char *join_char(char *result, char c)
{
    char    buffer[2];

    buffer[0] = c;
    buffer[1] = '\0';
    return (ft_strjoin_free(result, buffer));
}

/* 这是整个工程的总指挥。它排着队检查你输入的每一行字：
   如果是 $? 就请 handle_status 来帮忙；
   如果是 $变量名 就请 handle_variable 来帮忙；
   如果是普通字就直接贴上去。最后变出一串大家都看得懂的新文字。 */
char    *expand_heredoc_vars(t_minishell *ms, char *source)
{
    char    *result;
    int     step;

    result = ft_strdup("");
    while (*source)
    {
        step = 0;
        if (*source == '$' && *(source + 1) == '?')
            result = handle_status(ms, result, &step);
        else if (*source == '$' && (ft_isalpha(*(source + 1)) || *(source
                    + 1) == '_'))
            result = handle_variable(ms, source + 1, result, &step);
        else if (*source == '$')
            result = ft_strjoin_free(result, "$");
        else
            result = join_char(result, *source);
        if (step > 0)
            source += step + 1;
        else
            source++;
    }
    return (result);
}

