/* eq_stub.c -- 测试用：简单实现 equal_sign 函数。
 * 返回字符串中字符 '=' 的索引（0-based）。如果没有 '=' 返回字符串长度。
 * 这是临时桩，仅用于单模块测试，集成时应删除或替换为项目正式实现。
 */
#include <stddef.h>

int equal_sign(const char *s)
{
    int i = 0;
    if (!s) return 0;
    while (s[i])
    {
        if (s[i] == '=') return i;
        i++;
    }
    return i;
}
