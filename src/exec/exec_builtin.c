#include "../../include/exec.h"
#include "../../include/minishell.h"
#include "../../libft/libft.h"
#include "error.h"

// 判断命令是否需要在父进程中执行,一些改变进程状态的命令，如 cd, export, unset, exit 等，通常在父进程执行
static int is_builtin_parent(char *cmd)
{
    if (ft_strncmp(cmd, "cd", 2) == 0 || ft_strncmp(cmd, "export", 6) == 0 || ft_strncmp(cmd, "unset", 5) == 0 || ft_strncmp(cmd, "exit", 4) == 0)
    {
        return 1;
    }
    return 0;
}

// 在父进程中执行内置命令的逻辑:保存标准输入输出并重定向->执行命令之前，进行输入输出重定向->执行内置命令->恢复标准输入输出->返回状态码
static int run_builtin_parent_logic(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
    int ret = 0;

    t_fd_save save;
    if (save_std_fds(&save) != 0)
    {
        return 1;
    }
    if (dup_in_out_or_close(in_fd, out_fd) < 0)
    {
        restore_std_fds(&save);
        return 1;
    }
    ret = exec_builtin(node, &msh->env, msh);
    change_envp(msh->env, &msh->envp);
    exec_refresh_paths(msh);
    restore_std_fds(&save);
    return ret;
}

// 在子进程中执行内置命令的逻辑：保存标准输入输出并重定向->重定向输入输出->执行内置命令->恢复标准输入输出->返回状态码
static int run_builtin_child_logic(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
    int ret = 0;

    t_fd_save save;
    if (save_std_fds(&save) != 0)
    {
        return 1;
    }
    if (dup_in_out_or_close(in_fd, out_fd) < 0)
    {
        restore_std_fds(&save);
        return 1;
    }
    ret = exec_builtin(node, &msh->env, msh);
    restore_std_fds(&save);
    return ret;
}

/*
 *判断内置命令是否需要在父进程中执行
 *需要在父进程中执行的内置命令（例如 cd, export, exit 等）
 *对于不需要在父进程中执行的内置命令，创建子进程->在子进程中执行的逻辑->子进程退出时返回结果->父进程中等待子进程结束->获取子进程的退出状态
 *返回退出码
 */
int run_builtin_parent(t_minishell *msh, ast *node, int in_fd, int out_fd)
{
    pid_t pid;
    int ret = 1;

    if (is_builtin_parent(node->argv[0]))
        ret = run_builtin_parent_logic(msh, node, in_fd, out_fd);
    else
    {
        pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            return 1;
        }
        if (pid == 0)
        {
            ret = run_builtin_child_logic(msh, node, in_fd, out_fd);
            exit(ret);
        }
        else
        {
            int status;
            if (waitpid(pid, &status, 0) > 0)
                set_status_from_wait(msh, status);
        }
    }
    return ret;
}