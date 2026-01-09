/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:23:39 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/09 15:13:33 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "build_in.h"
#include "minishell.h"

/**
 * @brief 确定 cd 命令的目标路径并执行初步参数校验。
 * * 决策逻辑：
 * 1. 无参数 (cd): 尝试跳转至 HOME 环境变量定义的路径。
 * - 若 HOME 未设置，则报错并返回 NULL。
 * 2. 参数过多 (cd arg1 arg2 ...): 报错“too many arguments”并返回 NULL。
 * 3. 单个参数 (cd path): 将该参数作为目标路径返回。
 * * 注意：此处直接使用 getenv("HOME")。在 Minishell 进阶实现中，
 * 建议使用自定义的环境变量查询函数 (如 get_env_value)，以确保
 * 获取的是 Shell 内部修改后的环境状态。
 * * @param argv 命令参数数组（argv[0] 是 "cd"）。
 * @return char* 返回确定的目标路径指针，若校验失败则返回 NULL。
 */
static char *cd_get_target(char **argv)
{
    char *target;

    if (!argv[1])
    {
        target = getenv("HOME");
        if (!target)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return NULL;
        }
    }
    else if (argv[2])
    {
        ms_put3("cd: ", "too many arguments", "\n");
        return NULL;
    }
    else
        target = argv[1];
    return target;
}

/**
 * @brief 获取并保存当前工作目录（CWD）。
 * * 在执行目录切换（chdir）之前调用此函数：
 * 1. 核心调用：使用 getcwd() 系统调用获取当前绝对路径。
 * 2. 错误处理：如果获取失败（例如当前目录被删除或权限不足），使用 perror 打印系统错误信息。
 * 3. 状态返回：成功返回 0，失败返回 -1。
 * * 该函数的主要目的是为后续更新环境变量 OLDPWD（上一次工作目录）提供原始数据。
 * * @param cwd  用于存储路径字符串的缓冲区。
 * @param size 缓冲区的大小（通常为 PATH_MAX）。
 * @return int 成功返回 0，失败返回 -1。
 */
static int cd_get_oldcwd(char *cwd, size_t size)
{
    if (!getcwd(cwd, size))
    {
        perror("getcwd");
        return -1;
    }
    return 0;
}

/**
 * @brief 执行物理目录切换并处理可能的失败情况。
 * * 1. 系统调用：调用 chdir(target) 尝试更改当前进程的工作目录。
 * 2. 错误捕获：如果返回值不为 0，表示切换失败（通常原因为路径不存在或权限不足）。
 * 3. 错误反馈：仿照 minishell 格式向标准错误 (stderr) 输出提示信息。
 * 4. 状态返回：成功返回 0，失败返回 -1。
 * * @param target 经过校验后的目标路径字符串。
 * @return int 切换成功返回 0，失败返回 -1。
 */
static int cd_change_dir(const char *target)
{
    if (chdir(target) != 0)
    {
        fprintf(stderr, "minishell: cd: %s: No such file or directory\n", target);
        return -1;
    }
    return 0;
}

/**
 * @brief cd 内置命令的入口函数。
 * * 执行流程：
 * 1. 目标确定：通过 cd_get_target 处理参数（处理 HOME、路径或参数过多）。
 * 2. 备份旧路径：获取当前目录并存入 cwd 缓冲区，作为未来的 OLDPWD。
 * 3. 物理切换：调用 chdir 改变进程的当前工作目录。
 * 4. 更新环境变量：
 * - 将备份的旧路径设置到 OLDPWD。
 * - 获取跳转后的新绝对路径并设置到 PWD。
 * * @param argv 命令参数数组。
 * @param env  指向环境变量链表的指针地址。
 * @return int 成功返回 0，任何阶段失败则返回 1。
 */
int ft_cd(char **argv, t_env **env)
{
    char cwd[4096];
    char *target;

    target = cd_get_target(argv);
    if (!target)
        return 1;

    if (cd_get_oldcwd(cwd, sizeof(cwd)) != 0)
        return 1;

    if (cd_change_dir(target) != 0)
        return 1;

    env_set(env, "OLDPWD", cwd);
    if (!getcwd(cwd, sizeof(cwd)))
    {
        perror("getcwd");
        return 1;
    }
    env_set(env, "PWD", cwd);
    return 0;
}
