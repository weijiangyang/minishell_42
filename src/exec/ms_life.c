/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_life.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 02:48:36 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 18:29:54 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/build_in.h"
#include "../include/lexer.h"
#include "../include/minishell.h"
#include <stdlib.h>
#include <unistd.h>

void ms_child_exit(t_minishell *msh, t_ast *root, int code)
{
	if (root)
		free_ast(root);
	ms_clear(msh);
	exit(code);
}
