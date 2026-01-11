/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_nosubshell_cmd_redir_list.c                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:53:10 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/05 13:53:13 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parse.h"

/**
 * @brief 处理重定向 Token 的解析分支。
 * * 该函数是 build_redir 的封装，用于在解析循环中处理检测到的重定向符号。
 * 1. 调用 build_redir 消耗操作符和文件名 Token。
 * 2. 如果构建成功，将重定向节点挂载到当前命令的重定向链表中。
 * 3. 如果失败（如重定向后缺少文件名），则返回 0 以触发上层解析器的错误处理。
 * * @param cur        指向当前词法 Token 流指针的地址。
 * @param redir      指向当前 t_ast 节点重定向链表头指针的地址。
 * @param minishell  指向全局上下文结构体。
 * @return int       解析并构建成功返回 1；发生语法错误或内存失败返回 0。
 */
static int handle_redir_token(t_lexer **cur, t_redir **redir, t_minishell *minishell)
{
	int result;

	result = build_redir(cur, redir, minishell);
	if (!result)
		return 0;
	return 1;
}

/**
 * @brief 处理单词 Token 并将其添加到参数链表中。
 * * 该函数执行以下原子操作：
 * 1. 使用 consume_token 从词法流中提取当前的单词 Token。
 * 2. 获取该 Token 的原始字符串 (raw)。
 * 3. 调用 create_argv 创建一个新的命令参数节点。
 * 4. 使用 ft_lstadd_back 将新节点挂载到 argv_cmd 链表的末尾。
 * * @param cur       指向当前词法 Token 流指针的地址。
 * @param argv_cmd  指向临时命令参数链表头指针的地址。
 */
static void handle_word_token(t_lexer **cur, t_cmd **argv_cmd)
{
	ft_lstadd_back(argv_cmd, create_argv(consume_token(cur)->raw));
}

/**
 * @brief 检查是否既没有命令参数也没有重定向操作。
 * * 该函数用于识别“空命令”或语法无效的情况：
 * 1. 如果参数链表 (argv_cmd) 和重定向链表 (redir) 均为空，说明解析未产生有效动作。
 * 2. 状态码保护：如果当前的退出状态不是 130（即不是被 Ctrl+C 中断的），
 * 则将其设置为 2（标准语法错误状态码）。
 * * @param argv_cmd   参数链表指针。
 * @param redir      重定向链表指针。
 * @param minishell  指向全局结构体，用于更新退出状态。
 * @return int       若两者皆空返回 1（表示空指令）；否则返回 0。
 */
static int has_no_argv_and_no_redir(t_cmd *argv_cmd, t_redir *redir, t_minishell *minishell)
{
	if (!argv_cmd && !redir)
	{
		if (minishell->lt_ast_exit_status != 130)
			minishell->lt_ast_exit_status = 2;
		return 1;
	}
	return 0;
}

/**
 * @brief 解析普通命令及其重定向列表，并填充 t_ast 节点。
 * * 该函数进入一个循环，根据 Token 类型执行以下操作：
 * 1. 如果是单词 (TOK_WORD)：调用 handle_word_token 将其存入临时参数链表。
 * 2. 如果是重定向符：调用 handle_redir_token 解析操作符及其目标。
 * 3. 其他情况（如管道符或结尾）：停止解析。
 * * 循环结束后，它会：
 * - 检查是否解析到了有效内容。
 * - 将临时参数链表转换为以 NULL 结尾的字符串数组 (argv)。
 * * @param cur        指向当前词法 Token 流指针的地址。
 * @param node       当前正在构建的 t_ast 节点（NODE_CMD 类型）。
 * @param minishell  指向全局上下文结构体。
 * @return t_ast* 成功填充返回 node；若无有效内容或构建失败返回 NULL。
 */
t_ast *parse_normal_cmd_redir_list(t_lexer **cur, t_ast *node, t_minishell *minishell)
{
	t_lexer *pt;
	t_redir *redir = NULL;
	t_cmd *argv_cmd = NULL;

	if (!cur || !node)
		return NULL;
	node->type = NODE_CMD;
	while ((pt = peek_token(cur)) != NULL)
	{
		if (is_redir_token(pt))
		{
			if (!handle_redir_token(cur, &redir, minishell))
				return NULL;
		}
		else if (pt->tokentype == TOK_WORD)
			handle_word_token(cur, &argv_cmd);
		else
			break;
	}
	if (has_no_argv_and_no_redir(argv_cmd, redir, minishell))
		return NULL;
	node->redir = redir;
	node->argv = build_argvs(argv_cmd, redir, node);
	return node;
}
