/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_redir_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:18:45 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:18:51 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"
/**
 * @brief 创建并初始化一个新的重定向节点 (t_redir)。
 * * 该函数分配内存，并将 Lexer 识别到的 Token 类型转换为执行器识别的重定向类型。
 * 同时，它会通过 ft_strdup 复制文件名或 Heredoc 的限定符。
 * * @param type    词法分析后的 Token 类型 (tok_type)。
 * @param content 重定向指向的文件名或 Heredoc 终止符。
 * @return t_redir* 指向新创建节点的指针，若分配失败则返回 NULL。
 */
static t_redir *create_redir(tok_type type, char *content)
{
	t_redir *new_node;

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
 * @brief 将新的重定向节点添加到链表末尾。
 * * 该函数处理两种情况：
 * 1. 如果链表为空，则将新节点设为头节点。
 * 2. 如果链表不为空，则遍历至最后一个节点，并将其 next 指针指向新节点。
 * * @param lst      指向重定向链表头指针的指针（以便修改头指针本身）。
 * @param new_node  要添加的新重定向节点指针。
 */
static void redirlst_add_back(t_redir **lst, t_redir *new_node)
{
	t_redir *tmp;

	tmp = NULL;
	if (!lst || !new_node)
		return;
	if (!*lst)
	{
		*lst = new_node;
		return;
	}
	tmp = *lst;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

/**
 * @brief 消耗并提取一组重定向对（操作符 + 目标文件）。
 * * 该函数从词法流中连续提取两个 Token：
 * 1. 提取操作符 Token（<, >, <<, >>）。
 * 2. 提取紧随其后的文件名 Token，并验证其类型是否为 TOK_WORD。
 * * @param cur      指向当前词法流指针的指针（执行后会移动）。
 * @param op       用于存储提取出的操作符 Token 的指针。
 * @param filetok  用于存储提取出的文件名 Token 的指针。
 * @param ms       指向全局 minishell 结构体，用于设置错误状态码。
 * @return int     成功提取成对信息返回 1；若发生语法错误（缺少文件）则返回 0。
 */
static int consume_redir_pair(t_lexer **cur, t_lexer **op, t_lexer **filetok,
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
			ms->lt_ast_exit_status = 2;
		return (0);
	}
	return (1);
}

/**
 * @brief 构造重定向节点并将其加入链表。
 * * 该函数协同多个子模块工作：
 * 1. 验证词法流中是否存在有效的重定向对。
 * 2. 根据 Token 类型决定使用原始文本还是处理后的文本（用于处理 Heredoc 的变量展开）。
 * 3. 创建重定向节点并记录 Heredoc 是否包含引号（影响后续是否进行变量展开）。
 * 4. 将新节点安全地挂载到重定向链表的末尾。
 * * @param cur        指向当前词法流指针的指针。
 * @param redir_list 指向重定向链表头指针的地址。
 * @param ms         指向全局 minishell 结构体（用于错误状态记录）。
 * @return int       成功构造并添加返回 1；发生语法错误或内存分配失败返回 0。
 */
int build_redir(t_lexer **cur, t_redir **redir_list, t_minishell *ms)
{
	t_lexer *op;
	t_lexer *filetok;
	t_redir *new_redir;
	char *text;

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
	if (op->tokentype == TOK_HEREDOC)
		new_redir->quoted = filetok->had_quotes;
	redirlst_add_back(redir_list, new_redir);
	return (1);
}
