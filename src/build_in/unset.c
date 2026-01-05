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

// 删除指定的环境变量
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
