/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:17:29 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/11 20:31:46 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

static int	check_consecutive_pipes(t_lexer **cur)
{
	t_lexer	*pt;

	pt = peek_token(cur);
	if (pt && pt->next && pt->next->tokentype == TOK_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		return (-1);
	}
	return (0);
}

static t_ast	*create_pipe_node(t_ast *left, t_ast *right, int *n_pipes)
{
	t_ast	*node;

	node = ft_calloc(1, sizeof(t_ast));
	if (!node)
		return (NULL);
	node->type = NODE_PIPE;
	node->left = left;
	node->right = right;
	(*n_pipes)++;
	return (node);
}

static t_ast	*parse_pipeline_loop(t_lexer **cur, t_ast **left,
					int *n_pipes, t_minishell *msh)
{
	t_ast	*right;
	t_ast	*node;

	while (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
	{
		if (check_consecutive_pipes(cur) == -1)
		{
			free_ast(*left);
			return (NULL);
		}
		consume_token(cur);
		right = parse_simple_cmd_redir_list(cur, msh);
		if (!right)
		{
			ms_err_syntax_unexpected("newline");
			msh->last_exit_status = 2;
			free_ast(*left);
			return (NULL);
		}
		node = create_pipe_node(*left, right, n_pipes);
		if (!node)
			return (free_ast(*left), free_ast(right), NULL);
		*left = node;
	}
	return (*left);
}

t_ast	*parse_pipeline(t_lexer **cur, t_minishell *minishell)
{
	t_ast	*left;
	t_ast	*result;
	int		n_pipes;

	if (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		return (NULL);
	}
	left = parse_simple_cmd_redir_list(cur, minishell);
	if (!left)
		return (NULL);
	n_pipes = 0;
	result = parse_pipeline_loop(cur, &left, &n_pipes, minishell);
	if (!result)
		return (NULL);
	minishell->n_pipes = n_pipes;
	return (result);
}
