/* src/tests/test_lexer.c
 *
 * Lexer 单元测试驱动（增强版）
 * 支持命令行参数指定单个用例（1-based），不传参数则运行全部。
 *
 * 用法：
 *   make test_lexer   （或用 gcc 直接编译）
 *   ./test_lexer      # 运行全部
 *   ./test_lexer 2    # 只运行第二个用例（1-based）
 *
 * 每个测试用例旁边都写明了“在测什么”。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minishell.h"
#include "lexer.h"

/* 把 token type 转成人可读名字（匹配 include/lexer.h 中的 tok_type） */
static const char *token_name(tok_type t)
{
	if (t == TOK_WORD) return "WORD";
	if (t == TOK_PIPE) return "PIPE";
	if (t == TOK_REDIR_OUT) return "GREAT";
	if (t == TOK_APPEND) return "GREAT_GREAT";
	if (t == TOK_REDIR_IN) return "LESS";
	if (t == TOK_HEREDOC) return "LESS_LESS";
	if (t == TOK_AMP) return "AMP";
	if (t == TOK_AND) return "AND_AND";
	if (t == TOK_SEMI) return "SEMI";
	if (t == TOK_LPAREN) return "LPAREN";
	if (t == TOK_RPAREN) return "RPAREN";
	if (t == TOK_OR) return "OR_OR";
	if (t == TOK_END) return "END";
	return "UNKNOWN";
}

/* 打印链表详信息并清理（同时展示 raw / had_quotes / quoted_by） */
static void dump_and_clear_tokens(t_lexer **lst)
{
	t_lexer *cur = *lst;

	if (!cur)
	{
		printf("  (no tokens)\n");
		return;
	}
	while (cur)
	{
		printf("  idx=%d type=%s", cur->idx, token_name(cur->tokentype));
		if (cur->str)
			printf(" str=\"%s\"", cur->str);
		if (cur->raw)
			printf(" raw=\"%s\"", cur->raw);
		printf(" had_quotes=%d", cur->had_quotes);
		printf(" quoted_by=%d", cur->quoted_by);
		printf("\n");
		cur = cur->next;
	}
	clear_list(lst); /* 释放链表内存（你的实现应负责释放 raw/str） */
	*lst = NULL;
}

int main(int argc, char **argv)
{
	/* 用例数组：每条后面用注释说明在测试什么 */
	const char *cases[] = {
		"echo hello; echo world",    /* 1: 测试分号分隔与两个命令的 token 化（;） */
		"ls | wc -l",                /* 2: 测试管道连接（|）及后续参数解析 */
		"cmd && other",              /* 3: 测试逻辑 AND 运算符 (&&) 的识别 */
		"true || false",             /* 4: 测试逻辑 OR 运算符 (||) 的识别 */
		"echo \"a&b\"",              /* 5: 测试引号内包含特殊字符 &，应被视为 WORD 内容而非 AMP */
		"echo 'a b' c",              /* 6: 单引号中保留空白（单词内），并测试后续参数分隔 */
		"echo \"a b\" c",            /* 7: 双引号中保留空白并测试后续参数分隔 */
		"echo (sub)",                /* 8: 括号分离（LPAREN / RPAREN） */
		"cat <<EOF\nline\nEOF",      /* 9: HEREDOC token 测试（注意：这是 token 层识别 '<<'，实际 heredoc 读入在 parser/exec） */
		"echo \"unclosed",           /* 10: 测试未闭合双引号 —— lexer 应返回 NEED_MORE 或等价状态 */
		"echo 'unclosed",            /* 11: 测试未闭合单引号 —— lexer 应返回 NEED_MORE 或等价状态 */
		NULL
	};
	int i = 0;
	int only = -1;
	int retcode = 0;

	if (argc > 1)
	{
		only = atoi(argv[1]) - 1; /* 1-based -> 0-based */
		if (only < 0)
		{
			fprintf(stderr, "Usage: %s [case_index]\ncase_index is 1-based\n", argv[0]);
			return (2);
		}
	}

	printf("== Lexer unit tests ==\n");

	while (cases[i])
	{
		t_minishell ms;
		memset(&ms, 0, sizeof(ms));

		/* 如果指定单测，跳过其它 */
		if (only != -1 && i != only)
		{
			i++;
			continue;
		}

		printf("\n-- Test %d --\nInput: [%s]\n", i + 1, cases[i]);

		/* 把测试字符串放到 ms.raw_line（handle_lexer 使用该字段） */
		ms.raw_line = strdup(cases[i]);
		if (!ms.raw_line)
		{
			perror("strdup");
			return (1);
		}
		ms.lexer = NULL;

#ifdef g_signal
		g_signal = 0;
#endif

		/* 调用被测试函数 */
		int res = handle_lexer(&ms);

		/* 处理返回值：1 表示 OK；LEX_NEED_MORE 表示未闭合引号需要更多输入（由 REPL 处理）；
		   0 表示失败（内存错误或被中断等） */
#ifdef LEX_NEED_MORE
		if (res == LEX_NEED_MORE)
#else
		if (res == 2) /* 如果项目里未定义 LEX_NEED_MORE，保守假定值 2 */
#endif
		{
			/* lexer 检测到未闭合引号的场景（续行/EOF 由上层处理） */
			printf("  handle_lexer returned NEED_MORE (unclosed quote detected)\n");
			/* 如果 t_minishell 定义了 lexer_unclosed_quote 字段，则显示是哪种引号 */
#ifdef LEX_UNCLOSED_QUOTE
			(void)0; /* no-op to avoid unused macro warning when present */
#endif
			/* 尝试打印 ms.lexer_unclosed_quote（若结构定义中存在该字段） */
			/* 这里用 offsetof check 是不方便的，简单尝试访问（若不存在编译会报错） */
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
			/* 如果用户在 t_minishell 中添加了 lexer_unclosed_quote 字段，该字段会被填充 */
			/* 否则这打印会在编译时报错，说明你的 t_minishell 与测试文件需保持同步 */
			if (ms.lexer_unclosed_quote)
				printf("    unclosed quote char: '%c'\n", ms.lexer_unclosed_quote);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
			/* 确保释放任何部分构建的链表（handle_lexer 在 NEED_MORE 情况下应已清理，但保险起见调用一次） */
			clear_list(&ms.lexer);
		}
		else if (res == 1)
		{
			printf("  handle_lexer returned OK, tokens:\n");
			dump_and_clear_tokens(&ms.lexer);
		}
		else
		{
			printf("  handle_lexer returned FAILURE (syntax error or interrupted)\n");
			clear_list(&ms.lexer);
			if (only != -1)
				retcode = 1;
		}

		free(ms.raw_line);

		if (only != -1)
			break;

		i++;
	}

	printf("\n== Tests finished ==\n");
	return retcode;
}
