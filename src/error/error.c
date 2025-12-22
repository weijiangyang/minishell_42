/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 17:30:03 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:17:56 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 18:00:00 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:00:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include <errno.h>
#include "../../include/error.h"
#include "../../libft/libft.h"
/*
** 函数作用：
** 向标准错误输出依次打印 3 段字符串（任意一段为 NULL 就跳过）。
** 用于拼出 bash 风格的错误信息：minishell: xxx: yyy
*/
static void	ms_put3(const char *a, const char *b, const char *c)
{
	if (a)
		ft_putstr_fd((char *)a, STDERR_FILENO);
	if (b)
		ft_putstr_fd((char *)b, STDERR_FILENO);
	if (c)
		ft_putstr_fd((char *)c, STDERR_FILENO);
}

/*
** 函数作用：
** 打印 “命令找不到” 的 bash 风格报错，并不负责退出码。
** 例：minishell: lszzz: command not found
*/
void	ms_err_cmd_not_found(const char *cmd)
{
	ms_put3("minishell: ", cmd, ": command not found\n");
}

/*
** 函数作用：
** 打印外部命令执行失败的 bash 风格报错：
** 例：minishell: ./a.out: Permission denied
*/
void	ms_err_exec(const char *name, int err)
{
	ms_put3("minishell: ", name, ": ");
	ms_put3(strerror(err), "\n", NULL);
}

/*
** 函数作用：
** 打印重定向打开文件失败的 bash 风格报错：
** 例：minishell: /root/nope: Permission denied
*/
void	ms_err_redir(const char *name, int err)
{
	ms_put3("minishell: ", name, ": ");
	ms_put3(strerror(err), "\n", NULL);
}

/*
** 函数作用：
** 打印系统调用失败的 bash 风格报错（等价于 perror，但格式统一）。
** 例：minishell: fork: Resource temporarily unavailable
*/
void	ms_perror(const char *ctx)
{
	ms_err_exec(ctx, errno);
}
