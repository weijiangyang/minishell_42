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

#include "../../include/minishell.h"
#include "../../include/error.h"
#include "../../libft/libft.h"
#include "../../include/build_in.h"

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