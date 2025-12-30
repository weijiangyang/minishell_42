#include "../../include/exec.h"
#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include "error.h"

/*
** 函数作用：只关闭“不是标准输入/输出/错误”的 fd，避免误关 stdin/stdout。
*/
static void	close_keep_std(int fd)
{
	if (fd > STDERR_FILENO)
		close(fd);
}

/*
** 函数作用：判断是否必须在父进程执行（会改变父进程状态的 builtin）。
*/
static int	is_builtin_parent(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	return (0);
}

/*
** 函数作用：在父进程执行 builtin，并正确支持重定向：
** 1) 保存标准输入输出
** 2) 按从左到右应用重定向（< > >> <<），失败就报错并返回 1
** 3) dup2 到 stdin/stdout，执行 builtin
** 4) 恢复标准输入输出
*/
static int	run_builtin_parent_logic(t_minishell *msh, ast *node, int in_fd,
		int out_fd)
{
	t_fd_save	save;
	int			new_in;
	int			new_out;
	int			ret;

	ret = 1;
	if (save_std_fds(&save) != 0)
		return (close_keep_std(in_fd), close_keep_std(out_fd), 1);
	new_in = in_fd;
	new_out = out_fd;
	if (apply_redir_list(node->redir, &new_in, &new_out) < 0)
		return (restore_std_fds(&save), 1);
	if (dup_in_out_or_close(new_in, new_out) < 0)
		return (restore_std_fds(&save), 1);
	ret = exec_builtin(node, &msh->env, msh);
	change_envp(msh->env, &msh->envp);
	exec_refresh_paths(msh);
	restore_std_fds(&save);
	return (ret);
}

/*
** 函数作用：在子进程执行 builtin（echo/pwd/env 等），并正确支持重定向。
** 子进程执行完就 exit，所以不需要保存/恢复标准输入输出。
*/
static int	run_builtin_child_logic(t_minishell *msh, ast *node, int in_fd,
		int out_fd)
{
	int	new_in;
	int	new_out;
	int	ret;

	new_in = in_fd;
	new_out = out_fd;
	if (apply_redir_list(node->redir, &new_in, &new_out) < 0)
		return (1);
	if (dup_in_out_or_close(new_in, new_out) < 0)
		return (1);
	ret = exec_builtin(node, &msh->env, msh);
	return (ret);
}

/*
** 函数作用：
** - 必须在父进程执行的 builtin：直接在父进程跑（cd/export/unset/exit）
** - 其他 builtin：fork 子进程跑，父进程 wait 得到退出码
*/
int	run_builtin_parent(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
	pid_t	pid;
	int		status;
	int		ret;

	status = 0;
	ret = 1;
	if (is_builtin_parent(node->argv[0]))
		return (run_builtin_parent_logic(msh, node, in_fd, out_fd));
	pid = fork();
	if (pid < 0)
		return (ms_perror("fork"), close_keep_std(in_fd),
			close_keep_std(out_fd), 1);
	if (pid == 0)
	{
		ret = run_builtin_child_logic(msh, node, in_fd, out_fd);
		exit(ret);
	}
	close_keep_std(in_fd);
	close_keep_std(out_fd);
	if (waitpid(pid, &status, 0) > 0)
		set_status_from_wait(msh, status);
	return (msh->last_exit_status);
}
