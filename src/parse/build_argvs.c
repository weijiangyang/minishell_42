/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_argvs.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:52:27 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:52:30 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/parse.h"
#include "../../libft/libft.h"


t_cmd *create_argv(char *str)
{
    char *dup;

    if (!str)
        return NULL;
    dup = ft_strdup(str);
    if (!dup)
        return NULL;
    return ft_lstnew(dup);
}


char **build_argvs(t_cmd *argv_cmd, t_redir *redir, ast *node)
{
    int size;
    char **argvs;
    int i;
    t_cmd *tmp;

    if (argv_cmd == NULL)
        return (NULL);
    size = ft_lstsize(argv_cmd);
    argvs = malloc((size + 1) * sizeof(char *));
    if (!argvs)
        return (free_redir_list(redir), free_argv_list(argv_cmd), free(node), NULL);
    i = 0;
    tmp = argv_cmd;
    while (tmp && i < size)
    {
        argvs[i++] = tmp->content;
        tmp = tmp->next;
    }
    argvs[i] = NULL;
    tmp = argv_cmd;
    free_t_cmd_node(tmp);
    return (argvs);
}
