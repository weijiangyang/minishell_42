/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:53:56 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/20 16:55:56 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/lexer.h"
#include "../../libft/libft.h"

// 作用：是否为空白字符。
// 参数：`c`。
// 逻辑：对照空格/制表符等返回 1/0。
int	is_space(char c)
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
		if (!str[start_i + j] || is_space(str[start_i + j]))
			break ;
		else if (str[start_i + j] == 34 || str[start_i + j] == 39)
			continue ; /* 修改：引号后紧跟引号，不要 j++，让下一轮去 match_quotes 吃掉它 */
		else
			j++;
	}
	return (j);
}

// 作用：对提取出的原始子串做**引号处理与属性标记**，并填入 `info`。
// 参数：`substr` 截取的原文；`info` 临时节点信息。
// 逻辑：调用 `remove_quotes_flag`，拿到去壳文本与“出现过单/双引号”的标志，记录到 `info`；
// 失败时清理并返回负值。
static int	process_word_data(char *substr, t_token_info *info)
{
	char	*clean;

	clean = NULL;
	clean = remove_quotes_flag(substr, &info->had_quotes, &info->quoted_single,
			&info->quoted_double);
	if (!clean)
	{
		info->raw = NULL;
		info->clean = substr;
		info->had_quotes = 0;
		info->quoted_single = 0;
		info->quoted_double = 0;
	}
	else
	{
		info->raw = substr;
		info->clean = clean;
	}
	return (1);
}

// 作用：根据 `info` 构造 `WORD` 节点并追加到链表。
// 参数：`info`、链表头地址。
// 逻辑：调用 `add_node(info, WORD, list)`；根据返回值判断是否成功，失败回滚需要的内存。
static int	finalize_word_node(t_token_info *info, t_lexer **list)
{
	if (!add_node(info, TOK_WORD, list))
	{
		if (info->raw)
			free(info->raw);
		else if (info->clean)
			free(info->clean);
		return (-1);
	}
	return (1);
}

/* handle_word: 新签名，见 lexer.h
 * 当发现未闭合引号时返回 LEX_UNCLOSED_QUOTE，并不做打印或 readline；
 * 其余行为保持原逻辑：成功返回消费长度 j；出错返回 -1。
 */
int	handle_word(char *str, int i, t_lexer **list, char *out_unclosed)
{
	int				j;
	char			*substr;
	t_token_info	info;

	j = 0;
	substr = NULL;
	/* 调用改造后的 calc_word_len，获取是否有未闭合引号 */
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
