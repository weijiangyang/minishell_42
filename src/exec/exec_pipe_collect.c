/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_collect.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 16:47:37 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 18:20:12 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

static int	pipe_resize(t_ast ***arr, int len, int *cap)
{
	t_ast	**new_arr;
	int		new_cap;
	int		i;

	new_cap = *cap;
	if (new_cap == 0)
		new_cap = 4;
	else
		new_cap = new_cap * 2;
	new_arr = (t_ast **)malloc(sizeof(t_ast *) * new_cap);
	if (new_arr == NULL)
		return (0);
	i = 0;
	while (i < len)
	{
		new_arr[i] = (*arr)[i];
		i++;
	}
	free(*arr);
	*arr = new_arr;
	*cap = new_cap;
	return (1);
}

static int	pipe_push(t_ast ***arr, int *len, int *cap, t_ast *node)
{
	if (*len >= *cap)
	{
		if (!pipe_resize(arr, *len, cap))
			return (0);
	}
	(*arr)[*len] = node;
	(*len)++;
	return (1);
}

static int	pipe_collect_rec(t_ast *node, t_ast ***arr, int *len, int *cap)
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

int	pipe_collect(t_ast *root, t_ast ***out_arr, int *out_n)
{
	t_ast	**arr;
	int		len;
	int		cap;

	arr = NULL;
	len = 0;
	cap = 0;
	if (!pipe_collect_rec(root, &arr, &len, &cap))
	{
		if (arr)
			free(arr);
		return (0);
	}
	*out_arr = arr;
	*out_n = len;
	return (1);
}
