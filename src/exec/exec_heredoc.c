/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:21:10 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:21:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"
#include "exec.h"

/**
 * @brief 递归遍历 AST 并预处理所有 Heredoc 节点。
 * * 为什么需要预处理？
 * 在执行诸如 `cat << EOF | grep a` 的命令时，Shell 必须先让用户输入内容，
 * 然后才能创建管道和分叉进程。
 * * 处理逻辑：
 * 1. 基础检查：如果节点为空，返回成功 (1)。
 * 2. 局部处理：遍历当前节点的重定向链表 (t_redir)，
 * 若发现 HEREDOC 类型，调用 handle_heredoc 进行交互读取。
 * 3. 递归遍历：分别处理左子树和右子树（如管道两侧的命令）。
 * 4. 错误中断：如果任何一个 Heredoc 被用户中断（如 Ctrl+C），
 * 立即停止后续所有处理并返回 0。
 * * @param node  当前遍历到的 AST 节点指针。
 * @param ms    指向全局上下文结构体。
 * @return int  全部处理成功或无 Heredoc 返回 1；中途失败/中断返回 0。
 */
int prepare_heredocs(ast *node, t_minishell *ms)
{
    t_redir *r;

    if (!node)
        return (1);
    r = node->redir;
    while (r)
    {
        if (r->type == HEREDOC)
        {
            if (handle_heredoc(r, ms) == -1)
                return (0);
        }
        r = r->next;
    }
    if (!prepare_heredocs(node->left, ms))
        return (0);
    if (!prepare_heredocs(node->right, ms))
        return (0);
    return (1);
}

