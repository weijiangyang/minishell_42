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

typedef enum s_tokens
{
    PIPE = 1,    // '|' 管道符：将前后命令通过管道连接 /
    GREAT,       //'>' 输出重定向（覆盖） /
    GREAT_GREAT, // '>>' 输出重定向（追加） /
    LESS,        //'<' 输入重定向（从文件读入） /
    LESS_LESS,   //'<<' here-doc（直到 delimiter） /
}                    t_tokens;

typedef struct s_lexer
{
    char            str;
    t_tokens        tokentype;
    int             idx;
    struct s_lexer  *prev;
    struct s_lexer  *next;
} t_lexer;

typedef struct s_general
{
    t_lexer *lexer;      // tête du lexer
    int some_value;      // exemple de membre
    char **env;          // exemple : environnement
    // ajoute ici tous les autres champs nécessaires
} t_general;


void getcwd_wrapped(char *buf, size_t size);
void *malloc_wrapper(size_t size);
void *realloc_wrapper(void *ptr, size_t size);
int   cell_exit(char **args);
int cell_env(char **args);
pid_t fork_wrapper();
void execvp_wrapper(const char *file, char *const av[]);
int wait_wrapper(int *status);
int cell_echo(char **args);

#endif