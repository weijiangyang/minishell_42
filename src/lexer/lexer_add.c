/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_add.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 22:12:34 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/20 16:31:09 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/lexer.h"
#include "../../libft/libft.h"

// 改了什么
// switch 改 if/else if
// 保持：符号 token 会 strdup 出 raw/str

/*
** 函数作用：
** 给“符号 token”提供它应该显示的文本，比如 TOK_PIPE -> "|"
**
** 参数含义：
** t：token 类型
**
** 返回值：
** 返回对应的字符串常量；没有就返回 NULL
*/
static const char	*token_symbol(tok_type t)
{
	if (t == TOK_PIPE)
		return ("|");
	if (t == TOK_REDIR_OUT)
		return (">");
	if (t == TOK_REDIR_IN)
		return ("<");
	if (t == TOK_APPEND)
		return (">>");
	if (t == TOK_HEREDOC)
		return ("<<");
	return (NULL);
}

/*
** 函数作用：
** 把 word token 里临时 info 的内容“搬进”新节点里（不 strdup）
**
** 参数含义：
** new：新建的 lexer 节点
** info：word token 的信息包（clean/raw/had_quotes 等）
*/
static void	init_node_info(t_lexer *new, t_token_info *info)
{
	new->str = NULL;
	new->raw = NULL;
	new->had_quotes = 0;
	new->quoted_by = 0;
	if (!info)
		return ;
	new->str = info->clean;
	new->raw = info->raw;
	new->had_quotes = info->had_quotes;
	if (info->quoted_single)
		new->quoted_by = new->quoted_by + 1;
	if (info->quoted_double)
		new->quoted_by = new->quoted_by + 2;
}

/*
** 函数作用：
** 分配并初始化一个 t_lexer 节点
** - word token：直接使用 info 里的指针（不再 strdup）
** - 符号 token：根据 token_symbol 分配 raw/str
**
** 参数含义：
** info：word 的信息包；符号 token 时传 NULL
** tokentype：token 类型
**
** 返回值：
** 成功返回新节点指针；失败返回 NULL
*/
t_lexer	*new_node(t_token_info *info, tok_type tokentype)
{
	t_lexer		*new;
	const char	*sym;
	static int	idx;

	new = NULL;
	sym = NULL;
	new = malloc(sizeof(*new));
	if (!new)
		return (NULL);
	init_node_info(new, info);
	if (info == NULL)
	{
		sym = token_symbol(tokentype);
		if (sym)
		{
			new->raw = ft_strdup(sym);
			if (!new->raw)
				return (free(new), NULL);
			new->str = ft_strdup(sym);
			if (!new->str)
				return (free(new->raw), free(new), NULL);
		}
	}
	new->tokentype = tokentype;
	new->idx = idx;
	idx = idx + 1;
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
void	list_add_back(t_lexer **lst, t_lexer *new)
{
	t_lexer	*last;

	if (!lst || !new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next)
		last = last->next;
	last->next = new;
	new->prev = last;
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
int	add_node(t_token_info *info, tok_type tokentype, t_lexer **list)
{
	t_lexer	*node;

	node = NULL;
	node = new_node(info, tokentype);
	if (!node)
		return (0);
	list_add_back(list, node);
	return (1);
}
