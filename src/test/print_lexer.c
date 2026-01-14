#include "minishell.h"
#include "lexer.h"
#include "stdio.h"

void	print_lexer(t_minishell *msh)
{
	t_lexer *lexer_tokens;
	t_lexer *tmp;

	lexer_tokens = msh->lexer;

	tmp = lexer_tokens;
	while (tmp)
	{
		char *raw = tmp->raw;
		printf("%s\n", raw);
		tmp = tmp->next;
	}
}