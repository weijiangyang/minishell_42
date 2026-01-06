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

#include "minishell.h"
#include "parse.h"
/**
 * @brief 创建并初始化一个新的命令节点。
 * * 该函数接收一个原始字符串，通过内存拷贝（ft_strdup）创建副本，
 * 并将其封装进一个新的 t_cmd 链表节点中。
 * * @param str 指向要作为命令参数处理的原始字符串的指针。
 * @return t_cmd* 返回指向新创建节点的指针；如果内存分配失败或输入为空，则返回 NULL。
 */
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

/**
 * @brief 将命令链表转换为以 NULL 结尾的字符串数组（argv 格式）。
 * * 该函数计算链表长度，分配内存，并将链表中的字符串指针转移到数组中。
 * 转换完成后，它会释放链表节点的结构体内存，但保留字符串内容。
 * * @param argv_cmd 包含参数字符串的链表头。
 * @param redir    重定向链表指针（用于出错时统一释放内存）。
 * @param node     AST 节点指针（用于出错时统一释放内存）。
 * @return char** 指向生成的字符串数组的指针，失败时返回 NULL。
 */
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
