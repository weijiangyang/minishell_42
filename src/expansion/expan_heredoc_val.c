#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"
#include "../../include/build_in.h"

static int ft_strcmp_1(const char *s1, const char *s2)
{
    int i = 0;

    if (!s1 || !s2)
        return (s1 - s2); // 可以加保护，视你习惯

    while (s1[i] && s2[i])
    {
        if ((unsigned char)s1[i] != (unsigned char)s2[i])
            return ((unsigned char)s1[i] - (unsigned char)s2[i]);
        i++;
    }
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

char *ft_strjoin_free(char *s1, const char *s2)
{
    size_t len1 = s1 ? strlen(s1) : 0;
    size_t len2 = s2 ? strlen(s2) : 0;
    char *res = malloc(len1 + len2 + 1);

    if (!res)
    {
        if (s1)
            free(s1);
        return NULL;
    }

    if (s1)
        memcpy(res, s1, len1);
    if (s2)
        memcpy(res + len1, s2, len2);
    res[len1 + len2] = '\0';

    if (s1)
        free(s1);

    return res;
}

// 获取环境变量
static char *get_env_value(t_minishell *ms, const char *key)
{
    t_env *env = ms->env;
    while (env)
    {
        if (ft_strcmp_1(env->key, key) == 0)
            return env->value;
        env = env->next;
    }
    return "";
}

// heredoc 专用变量展开
char *expand_heredoc_vars(t_minishell *ms, char *line)
{
    char *result;
    char *ptr;
    char buffer[4096]; // 临时 buffer
    int j;
    result = ft_strdup("");
    ptr = line;

    while (*ptr)
    {
        if (*ptr == '$')
        {
            ptr++;
            if (*ptr == '?') // 特殊变量 $?
            {
                char tmp[32];
                snprintf(tmp, sizeof(tmp), "%d", ms->last_exit_status);
                result = ft_strjoin_free(result, tmp); // 拼接
                ptr++;
            }
            else if (ft_isalpha(*ptr) || *ptr == '_') // 普通变量
            {
                j = 0;
                while (ft_isalnum(ptr[j]) || ptr[j] == '_')
                {
                    buffer[j] = ptr[j]; // 先赋值
                    j++;
                }
                buffer[j] = '\0';
                result = ft_strjoin_free(result, get_env_value(ms, buffer));
                ptr += j;
            }
            else
            {
                // $ 后面不是字母数字或 ?，直接保留 $
                result = ft_strjoin_free(result, "$");
            }
        }
        else
        {
            // 普通字符
            char tmp[2] = {*ptr, 0};
            result = ft_strjoin_free(result, tmp);
            ptr++;
        }
    }
    return result;
}