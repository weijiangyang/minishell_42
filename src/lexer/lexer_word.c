/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:53:56 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/30 01:15:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

// 作用：是否为空白字符。
// 参数：`c`。
// 逻辑：对照空格/制表符等返回 1/0。
int	check_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

/* calc_word_len: 增加 out_unclosed 参数用于在发现未闭合引号时标注是哪种引号
 * 返回值语义：
 *  - >=0 : 返回单词长度（消费的字符数）
 *  - LEX_UNCLOSED_QUOTE (-2) : 发现未闭合引号，并设置 *out_unclosed 为 '\'' 或 '"'
 *  - <0 且 != LEX_UNCLOSED_QUOTE : 其它错误（保持原有 -1 语义用于内存/异常）
 */
static int	calc_word_len(char *str, int start_i, char *out_unclosed)
{
	int	j;
	int	q_len;

	j = 0;
	if (out_unclosed)
		*out_unclosed = '\0';
	while (str[start_i + j] && !(is_token((unsigned char)str[start_i + j])))
	{
		q_len = match_quotes(start_i + j, str, 34); /* 双引号 " */
		if (q_len == -1)
		{
			if (out_unclosed)
				*out_unclosed = '"';
			return (LEX_UNCLOSED_QUOTE);
		}
		j += q_len;
		q_len = match_quotes(start_i + j, str, 39); /* 单引号 ' */
		if (q_len == -1)
		{
			if (out_unclosed)
				*out_unclosed = '\'';
			return (LEX_UNCLOSED_QUOTE);
		}
		j += q_len;
		if (!str[start_i + j] || check_space(str[start_i + j]))
			break ;
		else if (str[start_i + j] == 34 || str[start_i + j] == 39)
			continue ; /* 引号后紧跟引号，不要 j++，让下一轮去 match_quotes 吃掉它 */
		else
			j++;
	}
	return (j);
}

// 作用：对截取出的 word 子串做“去引号 + 记录引号属性”，写进 info。
// 重点：parser 会把 token->raw 复制进 AST->argv，后续 expander 依赖 raw 里的引号
// 来判断“是否允许 $ 展开”，所以 raw 必须保留原始引号。
static int	process_word_data(char *substr, t_token_info *info)
{
	char	*clean;

	clean = NULL;
	if (!info)
		return (0);
	clean = remove_quotes_flag(substr, &info->had_quotes, &info->quoted_single,
			&info->quoted_double);
	info->raw = substr;
	if (!clean)
	{
		info->clean = substr;
		info->had_quotes = 0;
		info->quoted_single = 0;
		info->quoted_double = 0;
		return (1);
	}
	info->clean = clean;
	return (1);
}

// 作用：把 info 变成一个 WORD token 节点，追加进 lexer 链表。
// 如果 add_node 失败，要把本次创建的内存释放掉（避免泄漏）。
static int	finalize_word_node(t_token_info *info, t_lexer **list)
{
	if (!add_node(info, TOK_WORD, list))
	{
		/*
		** 失败回滚：raw/clean 可能是同一块内存，也可能是两块。
		** - 无引号：raw == clean（同一块）→ 只 free 一次
		** - 有引号：raw(含引号) != clean(去引号后) → 两块都要 free
		*/
		if (info->raw && info->clean && info->raw != info->clean)
		{
			free(info->raw);
			free(info->clean);
		}
		else if (info->raw)
			free(info->raw);
		else if (info->clean)
			free(info->clean);
		return (-1);
	}
	return (1);
}

/*
** handle_word：
** - 成功：返回本次吃掉的字符数 j
** - 未闭合引号：返回 LEX_UNCLOSED_QUOTE（不在 lexer 内部 readline）
** - 失败：返回 -1
*/
int	handle_word(char *str, int i, t_lexer **list, char *out_unclosed)
{
	int				j;
	char			*substr;
	t_token_info	info;

	j = 0;
	substr = NULL;
	j = calc_word_len(str, i, out_unclosed);
	if (j == LEX_UNCLOSED_QUOTE)
		return (LEX_UNCLOSED_QUOTE);
	if (j < 0)
		return (-1);
	if (j == 0)
		return (0);
	substr = ft_substr(str, i, j);
	if (!substr)
		return (-1);
	process_word_data(substr, &info);
	if (finalize_word_node(&info, list) < 0)
		return (-1);
	return (j);
}
