/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_redir_argv.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 16:25:07 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/25 16:25:09 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"
/**
 * @brief 释放重定向链表并关闭关联的系统资源。
 * * 遍历链表执行以下操作：
 * 1. 检查是否为 HEREDOC 类型，并确保关闭已打开的文件描述符 (fd)。
 * 2. 释放存储文件名的动态内存 (filename)。
 * 3. 释放节点本身的内存，通过临时指针安全地移动到下一个节点。
 * * @param r 指向重定向链表起始节点的指针。
 */
void free_redir_list(t_redir *r)
{
    t_redir *next;

    while (r)
    {
        next = r->next;
        if (r->type == HEREDOC && r->heredoc_fd >= 0)
        {
            close(r->heredoc_fd);
            r->heredoc_fd = -1;
        }
        free(r->filename);
        free(r);
        r = next;
    }
}

/**
 * @brief 释放命令参数链表及其内容。
 * * 遍历链表并执行以下操作：
 * 1. 释放节点中存储的字符串内容 (content)。
 * 2. 释放节点结构体本身的内存。
 * 3. 使用临时指针安全地移动到下一个节点，防止非法内存访问。
 * * @param a 指向待释放参数链表 (t_cmd) 头部的指针。
 */
void free_argv_list(t_cmd *a)
{
    t_cmd *next;

    while (a)
    {
        next = a->next;
        free(a->content);
        free(a);
        a = next;
    }
}

/**
 * @brief 仅释放命令链表的节点容器，保留字符串内容。
 * * 该函数用于指针所有权转移的场景：
 * 1. 当链表中的 content 指针已被保存到其他数组（如 build_argvs 生成的 char **）时。
 * 2. 遍历并释放每个 t_cmd 结构体，但不调用 free(tmp->content)。
 * * @param argv_cmd 指向待销毁链表头部的指针。
 */
void free_t_cmd_node(t_cmd *argv_cmd)
{
    t_cmd *tmp;
    t_cmd *next;

    tmp = argv_cmd;
    while (tmp)
    {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
}
