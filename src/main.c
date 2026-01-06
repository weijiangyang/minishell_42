/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:20:01 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/28 17:36:11 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/build_in.h"
#include "../include/exec.h"
#include "../include/minishell.h"
#include "../include/repl.h"
#include <unistd.h>

/*
** 函数作用：
**   往标准错误输出一段固定错误提示。
**   这里不用 printf，是为了更稳定，避免格式化带来的坑。
*/
static void ms_puterr(const char *s)
{
    int i;

    i = 0;
    while (s[i])
        i++;
    write(2, s, i);
}

/*
** 函数作用：
**   用 envp 初始化 minishell 的环境相关数据。
**   - init_env：把 envp 变成链表 env
**   - change_envp：把 env 链表再生成一份 envp（给 execve 用）
*/
static int ms_init_env(t_minishell *ms, char **envp)
{
    ms->env = init_env(envp);
    if (!ms->env)
        return (0);
    ms->envp = NULL;
    change_envp(ms->env, &ms->envp);
    if (!ms->envp)
        return (0);
    return (1);
}

/*
** 函数作用：
**   初始化 ms 的常用字段，并准备 PATH 缓存。
** 返回：
**   1 表示成功；0 表示失败（比如 malloc 失败）。
*/
static int ms_init(t_minishell *ms, char **envp)
{
    ms->env = NULL;
    ms->envp = NULL;
    ms->paths = NULL;
    ms->lexer = NULL;
    ms->raw_line = NULL;
    ms->last_exit_status = 0;
    ms->lexer_need_more = 0;
    ms->lexer_unclosed_quote = 0;
    if (!ms_init_env(ms, envp))
        return (0);
    if (ensure_paths_ready(ms) != 0)
        return (0);
    return (1);
}

/*
** 函数作用：
**   退出前统一释放资源，避免内存泄露。
** 注意：
**   你的项目里没有 free_char_matrix，所以这里用 while 手动 free(char**)。
**   free_env 的参数是 t_env*，不是 t_env**，所以要传 ms->env。
*/
static void ms_clear(t_minishell *ms)
{
    int i;
    char **m;

    if (!ms)
        return;
    if (ms->lexer)
        clear_list(&ms->lexer);
    m = ms->paths;
    if (m)
    {
        i = 0;
        while (m[i])
        {
            free(m[i]);
            i++;
        }
        free(m);
        ms->paths = NULL;
    }
    m = ms->envp;
    if (m)
    {
        i = 0;
        while (m[i])
        {
            free(m[i]);
            i++;
        }
        free(m);
        ms->envp = NULL;
    }
    if (ms->env)
    {
        free_env(ms->env);
        ms->env = NULL;
    }
}

/*
** 函数作用：
**   程序入口。
** 重点：
**   - 题目要求 minishell 不接受命令行参数，所以 argc 必须等于 1。
*/
int main(int argc, char **argv, char **envp)
{
    t_minishell ms;

    (void)argv;
    if (argc != 1)
    {
        ms_puterr("minishell: no argument allowed\n");
        return (1);
    }
    setup_prompt_signals();
    if (!ms_init(&ms, envp))
    {
        ms_puterr("minishell: init failed\n");
        ms_clear(&ms);
        return (1);
    }
    repl_loop(&ms);
    ms_clear(&ms);
    return (ms.last_exit_status);
}
