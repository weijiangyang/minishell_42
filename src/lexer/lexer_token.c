/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 04:44:56 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/20 16:31:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"
#include "../../include/lexer.h"


// 改了什么
// 符号 token 改为 add_node(NULL, token, list)（关键修复）
// is_token 只识别 mandatory 需要的字符（| < >），不把 ; \ 等当语法
/*
** 函数作用：
** 判断一个字符是不是“需要单独当成符号 token”的字符。
** 这里只保留 minishell 必做的：| < >
**
** 参数含义：
** c：当前字符（比如 '|' '<' '>'）
**
** 返回值：
** 对应的 tok_type；如果不是符号就返回 0
*/
tok_type	is_token(int c)
{
	if (c == '|')
		return (TOK_PIPE);
	if (c == '>')
		return (TOK_REDIR_OUT);
	if (c == '<')
		return (TOK_REDIR_IN);
	return (0);
}

/*
** 函数作用：
** 处理双字符符号：>> 和 <<
**
** 参数含义：
** tokentype：当前位置的单字符 token 类型（> 或 <）
** next_char：下一个字符（用于判断是不是 > 或 <）
** list：token 链表头指针地址（把新 token 加进去）
**
** 返回值：
** 成功消费 2 个字符返回 2
** 失败返回 -1
** 不属于双字符返回 0
*/
static int	handle_double_token(tok_type tokentype, int next_char, t_lexer **list)
{
	if (tokentype == TOK_REDIR_OUT && is_token(next_char) == TOK_REDIR_OUT)
	{
		if (!add_node(NULL, TOK_APPEND, list))
			return (-1);
		return (2);
	}
	if (tokentype == TOK_REDIR_IN && is_token(next_char) == TOK_REDIR_IN)
	{
		if (!add_node(NULL, TOK_HEREDOC, list))
			return (-1);
		return (2);
	}
	return (0);
}

/*
** 函数作用：
** 处理一个符号 token（| < > 或 >> <<）并把它加入 token 链表。
**
** 参数含义：
** str：整行输入
** i：当前下标（从这里开始识别）
** list：token 链表头指针地址
**
** 返回值：
** 返回本次“消耗了多少字符”（1 或 2）
** 出错返回 -1
** 如果当前位置不是符号返回 0
*/
int	handle_token(char *str, int i, t_lexer **list)
{
	tok_type	token;
	int			next_char;
	int			res;

	token = is_token((unsigned char)str[i]);
	next_char = (unsigned char)str[i + 1];
	res = handle_double_token(token, next_char, list);
	if (res != 0)
		return (res);
	if (token != 0)
	{
		if (!add_node(NULL, token, list))
			return (-1);
		return (1);
	}
	return (0);
}

/*
** 函数作用：
** 从 i 开始匹配一段完整引号（'...' 或 "..."），返回这段长度。
**
** 参数含义：
** i：起点下标
** str：原字符串
** quote：引号字符（' 或 "）
**
** 返回值：
** 找到闭合引号：返回包含首尾引号在内的总长度
** 未闭合：返回 -1
** 当前位置不是该引号：返回 0
*/
int	match_quotes(int i, char *str, char quote)
{
	int	j;

	j = 0;
	if (str[i + j] == quote)
	{
		j = j + 1;
		while (str[i + j] != quote && str[i + j])
			j = j + 1;
		if (str[i + j] == quote)
			j = j + 1;
		else
			return (-1);
	}
	return (j);
}
