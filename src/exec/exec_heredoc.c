
#include "../../include/minishell.h"
#include "../../include/exec.h"

int prepare_heredocs(ast *node, t_minishell *ms)
{
    t_redir *r;

    if (!node)
        return (1);

    r = node->redir;
    while (r)
    {
        if (r->type == HEREDOC)
        {
            if (handle_heredoc(r, ms) == -1)
                return (0);
        }
        r = r->next;
    }

    if (!prepare_heredocs(node->left, ms))
        return (0);
    if (!prepare_heredocs(node->right, ms))
        return (0);

    return (1);
}

