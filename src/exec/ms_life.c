/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_life.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 02:48:36 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 18:10:06 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/build_in.h"
#include "../include/lexer.h"
#include "../include/minishell.h"
#include <stdlib.h>
#include <unistd.h>

void	ms_child_exit(t_minishell *msh, ast *root, int code)
{
	if (root)
		free_ast(root);
	ms_clear(msh);
	exit(code);
}
