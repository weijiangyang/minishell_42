#include "../../include/minishell.h"
#include "../../include/error.h"
#include "../../libft/libft.h"
#include "../../include/build_in.h"

/**
 * change_envp - 将链表中的环境变量转换为一个数组，并更新 envp 指针。
 * 
 * 该函数将一个环境变量链表（t_env 类型）中的每个键值对转换为 `key=value` 格式的字符串，并将这些字符串存储到 `envp` 数组中。
 * 如果 `envp` 指针为空，则为其分配足够的内存来存储所有环境变量。最终，envp 数组将以 `NULL` 结尾，标志着数组的结束。
 * 
 * @env: 指向链表头的指针，该链表包含所有环境变量，每个环境变量由 t_env 结构体表示。
 * @envp: 指向 envp 数组的指针，该数组存储 `key=value` 格式的环境变量字符串。
 *        如果 envp 为空，该函数将为其分配内存。
 * 
 * 返回: 无返回值，直接更新传入的 envp 指针。
 */

void free_char_matrix(char **matrix)
{
    int i = 0;
    if (!matrix)
        return;
    while (matrix[i])
    {
        free(matrix[i]);
        i++;
    }
    free(matrix);
}

void change_envp(t_env *env, char ***envp)
{
    int i = 0;
    t_env *tmp = env;

    // 1. 每次调用时，最安全的方法是彻底释放旧的 envp 并重建
    // 否则处理数组长度变化（增加/减少环境变量）会非常麻烦
    if (*envp != NULL)
    {
        free_char_matrix(*envp);
        *envp = NULL;
    }

    // 2. 计算当前链表长度
    while (tmp) {
        i++;
        tmp = tmp->next;
    }

    // 3. 重新全量分配
    *envp = malloc(sizeof(char*) * (i + 1));
    if (*envp == NULL)
        return;

    tmp = env;
    i = 0;
    while (tmp) {
        char *key_value = ft_strjoin(tmp->key, "=");
        (*envp)[i] = ft_strjoin(key_value, tmp->value);
        free(key_value);
        tmp = tmp->next;
        i++;
    }
    (*envp)[i] = NULL;
}