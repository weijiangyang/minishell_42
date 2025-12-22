/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 21:03:52 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/21 22:37:40 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/expander.h"
#include "../../libft/libft.h"
/*
** 函数作用：只去掉 *s 里的引号（不做 $ 展开），并替换原指针。
** 参数：s(指向字符串指针，函数内会 free 原串并换成新串)
** 返回：成功 1，失败 0
*/
static int	strip_quotes_inplace(char **s)
{
	char	*tmp;
	int		had_q;
	int		q_s;
	int		q_d;

	tmp = NULL;
	had_q = 0;
	q_s = 0;
	q_d = 0;
	if (!s || !*s)
		return (1);
	tmp = remove_quotes_flag(*s, &had_q, &q_s, &q_d);
	if (!tmp)
		tmp = ft_strdup(*s);
	if (!tmp)
		return (0);
	free(*s);
	*s = tmp;
	return (1);
}

/*
** 函数作用：展开 argv（统一规则：$ 展开 + 去引号）。
** 参数：msh(全局上下文), node(CMD 节点)
** 返回：成功 1，失败 0
*/
static int	expand_argv_inplace(t_minishell *msh, ast *node)
{
	int		i;
	char	*tmp;

	i = 0;
	while (node->argv && node->argv[i])
	{
		tmp = expander_str(msh, node->argv[i]);
		if (!tmp)
			return (0);
		node->argv[i] = tmp;
		i = i + 1;
	}
	return (1);
}

/*
** 函数作用：展开重定向链表的 filename。
** 规则：
**  - 普通重定向(< > >>)：$ 展开 + 去引号
**  - heredoc(<<) 的 delimiter：永远只去引号（不做 $ 展开）
** 参数：msh(全局上下文), r(重定向链表头)
** 返回：成功 1，失败 0
*/
static int	expand_redirs_inplace(t_minishell *msh, t_redir *r)
{
	char	*tmp;

	tmp = NULL;
	while (r)
	{
		if (r->filename && r->type == HEREDOC)
		{
			if (!strip_quotes_inplace(&r->filename))
				return (0);
		}
		else if (r->filename)
		{
			tmp = expander_str(msh, r->filename);
			if (!tmp)
				return (0);
			r->filename = tmp;
		}
		r = r->next;
	}
	return (1);
}

/*
** 函数作用：展开一个 CMD 节点：先 argv，再 redir。
** 参数：msh(全局上下文), node(CMD 节点)
** 返回：成功 1，失败 0
*/
int	expander_expand_cmd_node(t_minishell *msh, ast *node)
{
	if (!msh || !node || node->type != NODE_CMD)
		return (1);
	if (!expand_argv_inplace(msh, node))
		return (0);
	if (!expand_redirs_inplace(msh, node->redir))
		return (0);
	return (1);
}
