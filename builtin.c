#include "cell.h"
/**
 * @shell 的内置退出命令
 * 
 * @参数 args 不使用
 * @返回值： 1 (只为符合prototype)
 */
int    cell_exit(char **args)
{
    (void) args;
    exit(EXIT_SUCCESS);
    return (1);
}

/**
 * @用于打印当前进程的环境变量。通过访问外部变量 environ 来实现这一功能。
 * 
 * @参数： args （）
 * @返回值： 0 或 1；
 */
int cell_env(char **args)
{
    extern  char **environ;

    int i;

    i = 0;
    
    if (!environ)
        return (1);
    while (environ[i])
    {
        printf("%s\n", environ[i]);
        i++;
    }
    return (0);
}