/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:31:17 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:31:19 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

/**
 * @brief 预读当前 Token 而不移动流指针。
 * * 该函数允许解析器在不“消耗” Token 的情况下检查其内容：
 * 1. 检查传入的二级指针是否有效。
 * 2. 返回当前词法单元 (t_lexer) 的指针。
 * 3. 词法流指针 (*cur) 保持不变，供后续逻辑再次检查或消耗。
 * * @param cur 指向当前 Token 流指针的地址。
 * @return t_lexer* 返回当前 Token 节点的指针；若流已空或参数无效则返回 NULL。
 */
t_lexer *peek_token(t_lexer **cur)
{
    if (!cur)
        return NULL;
    return *cur;
}

/**
 * @brief 消耗当前 Token 并将流指针移动到下一个节点。
 * * 该函数实现了 Token 流的推进逻辑：
 * 1. 安全检查：确保传入的流指针及其指向的内容有效。
 * 2. 暂存当前：保存当前正在处理的 Token 指针，以便作为返回值。
 * 3. 指针推进：将词法流指针 (*cur) 更新为链表中的下一个节点。
 * 4. 返回：返回移动前保存的那个 Token 节点。
 * * @param cur 指向当前词法 Token 流指针的地址。
 * @return t_lexer* 返回被消耗掉的那个 Token 指针；若已到达流末尾则返回 NULL。
 */
t_lexer *consume_token(t_lexer **cur)
{
    if (!cur || !*cur)
        return NULL;

    t_lexer *old = *cur;      
    *cur = (*cur)->next;     
    return old;               
}

/**
 * @brief 验证当前 Token 类型并消耗它。
 * * 该函数用于强制性的语法匹配：
 * 1. 检查当前 Token 是否存在且其类型是否符合预期 (type)。
 * 2. 如果类型不匹配：打印语法错误信息并返回 NULL，中断解析。
 * 3. 如果类型匹配：调用 consume_token 推进流指针并返回该 Token。
 * * @param type 预期的 Token 类型（如 TOK_RPAREN, TOK_WORD 等）。
 * @param cur  指向当前词法 Token 流指针的地址。
 * @return t_lexer* 匹配成功返回被消耗的 Token；匹配失败返回 NULL。
 */
t_lexer *expect_token(tok_type type, t_lexer **cur, t_minishell *msh)
{
    if (!cur || !*cur || (*cur)->tokentype != type)
    {
        msh->parse_status = PARSE_SYNTAX_ERROR;
        fprintf(stderr, "Syntax error : expected token type %d\n", type);
        return NULL;
    }
    return consume_token(cur);
}

/**
 * @brief 判断给定的 Token 是否为重定向操作符。
 * * 该函数检查以下四种标准的 Shell 重定向类型：
 * 1. TOK_REDIR_IN  ('<'): 输入重定向。
 * 2. TOK_REDIR_OUT ('>'): 输出重定向。
 * 3. TOK_APPEND    ('>>'): 追加输出重定向。
 * 4. TOK_HEREDOC   ('<<'): 这里的文档重定向。
 * * @param pt 指向待检查的词法单元 (t_lexer) 的指针。
 * @return int 如果是重定向操作符返回 1；否则返回 0。
 */
int is_redir_token(t_lexer *pt)
{
    if (pt->tokentype == TOK_REDIR_IN || 
        pt->tokentype == TOK_REDIR_OUT || 
        pt->tokentype == TOK_APPEND || 
        pt->tokentype == TOK_HEREDOC)
        return 1;
    else
        return 0;
}

/**
 * @brief 安全的字符串复制函数。
 * * 相比于标准的 ft_strdup，该函数增加了以下保护：
 * 1. 输入检查：如果输入字符串指针 s 为空，直接返回 NULL，防止程序崩溃。
 * 2. 失败反馈：如果 ft_strdup 因为内存耗尽（malloc 失败）返回 NULL，
 * 则向标准错误输出报错信息。
 * * @param s 指向待复制原始字符串的指针。
 * @return char* 成功返回新分配的字符串副本；若输入为空或分配失败则返回 NULL。
 */
char *safe_strdup(const char *s, t_minishell *msh)
{
    if (!s) return NULL;
    char *p = ft_strdup(s);
    if (!p)
    {
        msh->parse_status = PARSE_SYNTAX_ERROR;
        fprintf(stderr, "memory error: strdup failed\n");
    }
    return p;
}



