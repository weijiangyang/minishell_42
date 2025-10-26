#ifndef CELL_H
#define CELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024

#endif

//内置命令结构体
typedef struct  s_builtin
{
    char *builtin_name;
    int (*foo)(char **);
}   t_builtin;

void getcwd_wrapped(char *buf, size_t size);
void *malloc_wrapper(size_t size);
void *realloc_wrapper(void *ptr, size_t size);
int   cell_exit(char **args);
int cell_env(char **args);
pid_t fork_wrapper();
void execvp_wrapper(const char *file, char *const av[]);
int wait_wrapper(int *status);

#endif