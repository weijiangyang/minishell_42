/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_collect.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 16:47:37 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 18:07:19 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

/*
** 函数作用：动态数组扩容（容量翻倍，最小从 4 开始）
** 参数含义：arr(数组地址), len(已有元素个数), cap(容量地址)
** 返回值：成功 1，失败 0
*/

static int pipe_resize(t_ast ***arr, int len, int *cap)
{
	t_ast **new_arr;
	int new_cap;
	int i;

	new_cap = *cap;
	if (new_cap == 0)
		new_cap = 4;
	else
		new_cap = new_cap * 2;
	new_arr = (t_ast **)malloc(sizeof(**arr) * new_cap);
	if (new_arr == NULL)
		return (0);
	i = 0;
	while (i < len)
	{
		new_arr[i] = (*arr)[i];
		i = i + 1;
	}
	free(*arr);
	*arr = new_arr;
	*cap = new_cap;
	return (1);
}

/*
** 函数作用：把一个命令节点塞进数组，不够大就扩容
** 参数含义：arr/len/cap(动态数组三件套), node(要加入的命令节点)
** 返回值：成功 1，失败 0
*/
static int pipe_push(t_ast ***arr, int *len, int *cap, t_ast *node)
{
	if (*len >= *cap)
		if (!pipe_resize(arr, *len, cap))
			return (0);
	(*arr)[*len] = node;
	*len = *len + 1;
	return (1);
}

/*
** 函数作用：递归摊平 PIPE 树：a|b|c -> [a,b,c]
** 参数含义：node(t_ast 当前节点), arr/len/cap(动态数组三件套)
** 返回值：成功 1，失败 0
*/
static int pipe_collect_rec(t_ast *node, t_ast ***arr, int *len, int *cap)
{
	if (!node)
		return (1);
	if (node->type != NODE_PIPE)
		return (pipe_push(arr, len, cap, node));
	if (!pipe_collect_rec(node->left, arr, len, cap))
		return (0);
	if (!pipe_collect_rec(node->right, arr, len, cap))
		return (0);
	return (1);
}

/*
** 函数作用：对外接口：把 PIPE 根摊平，输出数组和段数
** 参数含义：root(PIPE 根), out_arr(输出数组), out_n(输出段数)
** 返回值：成功 1，失败 0
*/
int pipe_collect(t_ast *root, t_ast ***out_arr, int *out_n)
{
	t_ast **arr;
	int len;
	int cap;

	arr = NULL;
	len = 0;
	cap = 0;
	if (!pipe_collect_rec(root, &arr, &len, &cap))
		return (free(arr), 0);
	*out_arr = arr;
	*out_n = len;
	return (1);
}
