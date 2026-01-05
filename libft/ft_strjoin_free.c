#include "libft.h"

char *ft_strjoin_free(char *s1, const char *s2)
{
    size_t len1;
    size_t len2;
    char *res;

    len1 = s1 ? ft_strlen(s1) : 0;
    len2 = s2 ? ft_strlen(s2) : 0;
    res = malloc(len1 + len2 + 1);
    if (!res)
    {
        if (s1)
            free(s1);
        return NULL;
    }
    if (s1)
        ft_memcpy(res, s1, len1);
    if (s2)
        ft_memcpy(res + len1, s2, len2);
    res[len1 + len2] = '\0';
    if (s1)
        free(s1);
    return res;
}
