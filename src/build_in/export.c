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
