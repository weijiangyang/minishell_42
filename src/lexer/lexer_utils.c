/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:30:24 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 18:46:30 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

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
const char	*token_symbol(tok_type t)
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
** 作用：检查是否有人按了 Ctrl+C，如果按了就清理掉已经切好的零件。
*/
int	detect_signal(t_lexer **list)
{
	if (g_signal == SIGINT)
	{
		clear_list(list);
		g_signal = 0;
		return (1);
	}
	return (0);
}

/*
** 作用：当程序出错时，负责把刚才申请的内存全部还给电脑（释放内存）。
*/
static void	release_word_memory(t_token_info *info)
{
	if (info->raw && info->clean && info->raw != info->clean)
	{
		free(info->raw);
		free(info->clean);
	}
	else if (info->raw)
		free(info->raw);
	else if (info->clean)
		free(info->clean);
}

/*
** 作用：完成最后一步，把准备好的单词信息正式塞进零件链表里。
*/
int	finalize_word_node(t_token_info *info, t_lexer **list)
{
	if (!add_node(info, TOK_WORD, list))
	{
		release_word_memory(info);
		return (-1);
	}
	return (1);
}
