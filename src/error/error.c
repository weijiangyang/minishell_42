/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 17:30:03 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/09 15:13:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/*
** 函数作用：把 3 段字符串按顺序打印到 stderr。
** 解释（初中生版）：
** 有些错误信息是 "前缀 + 命令 + 后缀" 拼起来的，
** 用这个函数就不用到处写很多重复的 ft_putstr_fd。
**
** 注意：
** 你们的 libft 里 ft_putstr_fd 参数是 char*，但我们手里是 const char*。
** 这里强转 (char *) 是为了通过编译（我们不会改动字符串内容）。
*/
void	ms_put3(const char *a, const char *b, const char *c)
{
	if (a)
		ft_putstr_fd((char *)a, STDERR_FILENO);
	if (b)
		ft_putstr_fd((char *)b, STDERR_FILENO);
	if (c)
		ft_putstr_fd((char *)c, STDERR_FILENO);
}

/*
** 函数作用：打印 “command not found” 错误，并尽量贴近 minishell 输出格式。
*/
void    ms_err_cmd_not_found(const char *cmd)
{
    ms_put3("minishell: ", NULL, NULL);
    ms_put3(cmd, ": command not found\n", NULL);
}

/*
** 函数作用：打印 execve 相关错误，例如 Permission denied / Is a directory 等。
*/
void    ms_err_exec(const char *cmd, int err)
{
    ms_put3("minishell: ", NULL, NULL);
    ms_put3(cmd, ": ", strerror(err));
    ms_put3("\n", NULL, NULL);
}


/*
** 函数作用：打印重定向打开文件失败的错误，例如 No such file or directory。
*/
void    ms_err_redir(const char *file, int err)
{
    ms_put3("minishell: ", NULL, NULL);
    ms_put3(file, ": ", strerror(err));
    ms_put3("\n", NULL, NULL);
}

void    ms_err_bad_interpreter(const char *cmd, const char *interp)
{
    ms_put3("minishell: ", NULL, NULL);
    ms_put3(cmd, ": ", NULL);
    ms_put3(interp, ": bad interpreter: Permission denied\n", NULL);
}
