/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_nosubshell_cmd_redir_list.c                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:53:10 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:53:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../include/parse.h"
#include "../../libft/libft.h"


static int handle_redir_token(t_lexer **cur, t_redir **redir, t_minishell *minishell)
{
    int result;

    result = build_redir(cur, redir, minishell);
    if (!result)
        return 0; // ❗ 终止解析
    return 1;
}

static void handle_word_token(t_lexer **cur, t_cmd **argv_cmd)
{
    ft_lstadd_back(argv_cmd, create_argv(consume_token(cur)->raw));
}

static int has_no_argv_and_no_redir(t_cmd *argv_cmd, t_redir *redir, t_minishell *minishell)
{
    if (!argv_cmd && !redir)
    {
        if (minishell->last_exit_status != 130)
            minishell->last_exit_status = 2;
        return 1;
    }
    return 0;
}

ast *parse_normal_cmd_redir_list(t_lexer **cur, ast *node, t_minishell *minishell)
{
    t_lexer *pt;
    t_redir *redir = NULL;
    t_cmd *argv_cmd = NULL;

    if (!cur || !node)
        return NULL;
    node->type = NODE_CMD;
    while ((pt = peek_token(cur)) != NULL)
    {
        if (is_redir_token(pt))
        {
            if (!handle_redir_token(cur, &redir, minishell))
                return NULL;
        }
        else if (pt->tokentype == TOK_WORD)
            handle_word_token(cur, &argv_cmd);
        else
            break;
    }
    if (has_no_argv_and_no_redir(argv_cmd, redir, minishell))
        return NULL;
    node->redir = redir;
    node->argv = build_argvs(argv_cmd, redir, node);
    return node;
}
