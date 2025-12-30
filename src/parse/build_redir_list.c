/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_redir_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 16:17:59 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/30 01:19:18 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/parse.h"
#include "../../libft/libft.h"

/**
 * create_redir
 * ------------------------------------------------------------
 * 目的：
 *   为解析阶段创建一个新的重定向节点（t_redir），并根据
 *   token 类型设置其重定向种类（输入、输出、追加、heredoc）。
 *   同时会复制文件名（或 delimiter）字符串，以保证内存独立性。
 *
 * 参数：
 *   @type    - 来自词法分析的 token 类型（tok_type）
 *              TOK_REDIR_IN, TOK_REDIR_OUT, TOK_APPEND, TOK_HEREDOC
 *
 *   @content - 重定向后跟随的目标文件名或 heredoc 的 delimiter。
 *              该字符串由 lexer 提供，这里会 strdup() 创建副本。
 *
 * 返回值：
 *   成功：返回新创建且初始化完毕的 t_redir 指针
 *   失败：返回 NULL（内存分配失败）
 *
 * 逻辑说明：
 *   1. 分配一个 t_redir 节点并初始化为 0。
 *   2. 使用 strdup() 复制 content，确保 redir 节点拥有自己的内存。
 *   3. 根据 token 类型设置节点的重定向类型：
 *         - TOK_REDIR_IN   -> `<`
 *         - TOK_REDIR_OUT  -> `>`
 *         - TOK_APPEND     -> `>>`
 *         - TOK_HEREDOC    -> `<<`
 *   4. heredoc_fd 初始化为 -1，表示暂未创建管道。
 *   5. 返回配置完成的节点。
 */
static t_redir	*create_redir(tok_type type, char *content)
{
	t_redir	*new_node;

	new_node = ft_calloc(1, sizeof(t_redir));
	if (!new_node)
		return (NULL);
	new_node->filename = ft_strdup(content);
	if (!content)
		return (NULL);
	if (!new_node->filename)
	{
		free(new_node);
		return (NULL);
	}
	new_node->next = NULL;
	new_node->heredoc_fd = -1;
	if (type == TOK_REDIR_IN)
		new_node->type = REDIR_INPUT;
	else if (type == TOK_REDIR_OUT)
		new_node->type = REDIR_OUTPUT;
	else if (type == TOK_APPEND)
		new_node->type = REDIR_APPEND;
	else if (type == TOK_HEREDOC)
		new_node->type = HEREDOC;
	return (new_node);
}

/**
 * redirlst_add_back
 * ------------------------------------------------------------
 * 目的：
 *   将一个新的重定向节点（t_redir）追加到重定向链表末尾。
 *   该链表保存所有与当前命令相关的重定向（<, >, >>, <<）。
 *
 * 参数：
 *   @lst       - 指向重定向链表头指针的地址（t_redir**）。
 *                如果链表为空，本函数会将 new_node 设为链表头。
 *
 *   @new_node  - 已分配并初始化好的 t_redir 节点。
 *                其 next 字段应由调用者保证为 NULL。
 *
 * 返回值：
 *   无（void）。
 *   若 lst 或 new_node 为 NULL，本函数将不进行任何操作。
 *
 * 逻辑：
 *   1. 检查参数有效性。
 *   2. 若链表为空，则直接将 new_node 设为头节点。
 *   3. 若链表非空，则遍历至尾节点，并将 new_node 挂在末尾。
 */

static void	redirlst_add_back(t_redir **lst, t_redir *new_node)
{
	t_redir	*tmp;

	tmp = NULL;
	if (!lst || !new_node)
		return ;
	if (!*lst)
	{
		*lst = new_node;
		return ;
	}
	tmp = *lst;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

// 作用：从 token 流里消费 "重定向符号 + 后面的 WORD"。
// 失败时：打印 bash 同款语法错误，并把 exit status 置为 2。
static int	consume_redir_pair(t_lexer **cur, t_lexer **op, t_lexer **filetok,
		t_minishell *ms)
{
	*op = consume_token(cur);
	if (!*op)
		return (0);
	*filetok = consume_token(cur);
	if (!*filetok || (*filetok)->tokentype != TOK_WORD)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
		if (ms)
			ms->last_exit_status = 2;
		return (0);
	}
	return (1);
}

// 作用：执行 heredoc 读取；失败时释放本次 new_redir，避免泄漏。
static int	heredoc_make(t_redir *new_redir, t_minishell *ms)
{
	if (handle_heredoc(new_redir, ms) == -1)
	{
		free(new_redir->filename);
		free(new_redir);
		return (0);
	}
	return (1);
}

// 作用：构建一个 redir 节点并追加到 redir_list。
// 关键点：
// - 普通重定向：用 filetok->raw（保留引号），交给 expander 决定是否展开 $。
// - heredoc：用 filetok->str（已去包裹引号），保证 delimiter 能正确匹配输入。
int	build_redir(t_lexer **cur, t_redir **redir_list, t_minishell *ms)
{
	t_lexer	*op;
	t_lexer	*filetok;
	t_redir	*new_redir;
	char	*text;

	op = NULL;
	filetok = NULL;
	new_redir = NULL;
	text = NULL;
	if (!consume_redir_pair(cur, &op, &filetok, ms))
		return (0);
	text = filetok->raw;
	if (op->tokentype == TOK_HEREDOC)
		text = filetok->str;
	new_redir = create_redir(op->tokentype, text);
	if (!new_redir)
		return (0);
	if (op->tokentype == TOK_HEREDOC && !heredoc_make(new_redir, ms))
		return (0);
	redirlst_add_back(redir_list, new_redir);
	return (1);
}
