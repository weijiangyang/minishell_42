/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 00:15:10 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/11 23:04:47 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "minishell.h"

static int	read_shebang(const char *path, char *buf)
{
	int		fd;
	ssize_t	n;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	n = read(fd, buf, 199);
	close(fd);
	if (n < 2)
		return (0);
	buf[n] = '\0';
	return (buf[0] == '#' && buf[1] == '!');
}

static int	try_print_bad_interpreter(const char *cmd, const char *path,
	int err)
{
	char	buf[200];
	int		i;
	int		j;

	if (!cmd || !path || err != EACCES)
		return (0);
	if (!read_shebang(path, buf))
		return (0);
	i = 2;
	while (buf[i] == ' ' || buf[i] == '\t')
		i++;
	j = i;
	while (buf[j] && buf[j] != ' ' && buf[j] != '\t' && buf[j] != '\n')
		j++;
	if (j == i)
		return (0);
	buf[j] = '\0';
	ms_err_bad_interpreter(cmd, &buf[i]);
	return (1);
}

static void	handle_exec_exit(t_minishell *msh, t_ast *root,
	char *path, char **av)
{
	int	err;
	int	code;

	err = errno;
	if (try_print_bad_interpreter(av[0], path, err))
	{
		free(path);
		ms_child_exit(msh, root, 126);
	}
	ms_err_exec(av[0], err);
	code = 127;
	if (err == EACCES || err == EISDIR)
		code = 126;
	free(path);
	ms_child_exit(msh, root, code);
}

static void	child_exec_external(t_minishell *msh, t_ast *node, t_ast *root)
{
	char	**argv;
	char	*path;

	argv = node->argv;
	if (!argv || !argv[0])
		ms_child_exit(msh, root, 0);
	change_envp(msh->env, &msh->envp);
	path = find_cmd_path(msh, argv[0]);
	if (!path)
	{
		ms_err_cmd_not_found(argv[0]);
		ms_child_exit(msh, root, 127);
	}
	if (path_is_dir(path))
	{
		ms_err_exec(argv[0], EISDIR);
		free(path);
		ms_child_exit(msh, root, 126);
	}
	execve(path, argv, msh->envp);
	handle_exec_exit(msh, root, path, argv);
}

void	child_exec_one(t_exec_ctx *ctx)
{
	int	new_in;
	int	new_out;

	new_in = ctx->in_fd;
	new_out = ctx->out_fd;
	if (!ctx->msh || !ctx->node || ctx->node->type != NODE_CMD)
		ms_child_exit(ctx->msh, ctx->root, 1);
	if (has_bad_heredoc(ctx->node->redir))
		ms_child_exit(ctx->msh, ctx->root, 1);
	if (new_in > 2 && ctx->node->argv && ctx->node->argv[0]
		&& is_builtin(ctx->node->argv[0]))
	{
		close(new_in);
		new_in = -1;
	}
	if (apply_redir_list(ctx->node->redir, &new_in, &new_out) < 0)
		ms_child_exit(ctx->msh, ctx->root, 1);
	close_all_heredoc_fds(ctx->root);
	if (dup_in_out_or_close(new_in, new_out) < 0)
		ms_child_exit(ctx->msh, ctx->root, 1);
	if (ctx->node->argv && ctx->node->argv[0] && is_builtin(ctx->node->argv[0]))
		ms_child_exit(ctx->msh, ctx->root,
			exec_builtin(ctx->node, &ctx->msh->env, ctx->msh));
	child_exec_external(ctx->msh, ctx->node, ctx->root);
}
