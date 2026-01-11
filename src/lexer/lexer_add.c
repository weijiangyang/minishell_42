/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_add.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 22:12:34 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 18:20:12 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

/*
** 函数作用：
** 把 word token 里临时 info 的内容“搬进”新节点里（不 strdup）
**
** 参数含义：
** new：新建的 lexer 节点
** info：word token 的信息包（clean/raw/had_quotes 等）
*/
static void init_node_info(t_lexer *new, t_token_info *info)
{
	new->str = NULL;
	new->raw = NULL;
	new->had_quotes = 0;
	new->quoted_by = 0;
	if (!info)
		return;
	new->str = info->clean;
	new->raw = info->raw;
	new->had_quotes = info->had_quotes;
	if (info->quoted_single)
		new->quoted_by = new->quoted_by + 1;
	if (info->quoted_double)
		new->quoted_by = new->quoted_by + 2;
}

/*
** 作用：给新节点起个名字。
** 如果没有提供信息，就根据类型（比如管道或重定向）自动填上符号。
*/
static int assign_node_name(t_lexer *new, tok_type type)
{
	const char *symbol;

	symbol = token_symbol(type);
	if (!symbol)
		return (1);
	new->raw = ft_strdup(symbol);
	if (!new->raw)
		return (0);
	new->str = ft_strdup(symbol);
	if (!new->str)
	{
		free(new->raw);
		return (0);
	}
	return (1);
}

/*
** 作用：创建一个新的“零件”（节点）。
** 准备好它的位置、编号和类型，方便后面串成链表。
*/
t_lexer *new_node(t_token_info *info, tok_type type)
{
	t_lexer *new;
	static int count = 0;

	new = malloc(sizeof(*new));
	if (!new)
		return (NULL);
	ft_bzero(new, sizeof(*new));
	init_node_info(new, info);
	if (info == NULL)
	{
		if (!assign_node_name(new, type))
			return (free(new), NULL);
	}
	new->tokentype = type;
	new->idx = count++;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}

/*
** 函数作用：
** 把一个节点加到双向链表末尾
**
** 参数含义：
** lst：链表头指针地址
** new：要追加的新节点
*/
void list_add_back(t_lexer **lst, t_lexer *new)
{
	t_lexer *lt_ast;

	if (!lst || !new)
		return;
	if (!*lst)
	{
		*lst = new;
		return;
	}
	lt_ast = *lst;
	while (lt_ast->next)
		lt_ast = lt_ast->next;
	lt_ast->next = new;
	new->prev = lt_ast;
}

/*
** 函数作用：
** 统一“创建节点并加入链表”的入口
**
** 参数含义：
** info：word 的信息包；符号 token 传 NULL
** tokentype：token 类型
** list：链表头指针地址
**
** 返回值：
** 成功返回 1，失败返回 0
*/
int add_node(t_token_info *info, tok_type tokentype, t_lexer **list)
{
	t_lexer *node;

	node = NULL;
	node = new_node(info, tokentype);
	if (!node)
		return (0);
	list_add_back(list, node);
	return (1);
}
