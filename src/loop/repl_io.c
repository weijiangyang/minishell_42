/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_io.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:17:36 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/28 17:22:33 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "repl.h"

/*
** 函数作用：
**   读取用户一行输入（readline）。
** 提示符规则：
**   - acc 有内容：说明在续行 → "> "
**   - acc 为空：主提示符 → "minishell$ "
** 非交互模式（stdin 不是 tty）：
**   - 不打印提示符（prompt 传空串）。
*/
char	*repl_read(char *acc)
{
	char	*prompt;

	prompt = "minishell$ ";
	if (acc)
		prompt = "> ";
	if (!isatty(STDIN_FILENO))
		prompt = "";
	return (readline(prompt));
}

/*
** 函数作用：
**   判断字符串里是否有非空白字符。
** 说明：
**   用户只输入空格/Tab/回车时，不应该当作一条命令。
*/
int	repl_has_text(const char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (!check_space((unsigned char)s[i]))
			return (1);
		i++;
	}
	return (0);
}

/*
** 函数作用：
**   把 line 拼到 acc 后面。
** 规则：
**   - acc 为空：acc = strdup(line)
**   - acc 非空：acc = acc + "\n" + line
*/
int	repl_join(char **acc, char *line)
{
	char	*tmp;

	if (!acc || !line)
		return (0);
	if (!*acc)
	{
		*acc = ft_strdup(line);
		return (*acc != NULL);
	}
	tmp = ft_strjoin(*acc, "\n");
	free(*acc);
	*acc = tmp;
	if (!*acc)
		return (0);
	tmp = ft_strjoin(*acc, line);
	free(*acc);
	*acc = tmp;
	return (*acc != NULL);
}

/*
** 函数作用：
**   释放 acc（累计输入缓冲区），并把指针置为 NULL。
** 为什么要这样写：
**   - 置 NULL 可以防止“二次 free”这种常见 bug。
*/
void	repl_free_acc(char **acc)
{
	if (!acc)
		return ;
	if (*acc)
		free(*acc);
	*acc = NULL;
}
