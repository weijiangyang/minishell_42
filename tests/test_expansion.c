/* src/expansion/test.c
 *
 * Expander 单元测试 + 可选的 valgrind 泄露检测支持
 * 
 gcc -g -O0 \
  -I include -I src/expansion -I src/lexer -I libft \
  src/expansion/expan_api.c \
  src/expansion/expan_env.c \
  src/expansion/expan_scan.c \
  src/expansion/expan_scan2.c \
  src/expansion/expan_utils.c \
  src/expansion/expan_list.c \
  src/expansion/test.c \
  src/expansion/eq_stub.c \
  src/lexer/lexer_remove_quotes.c \
  libft/libft.a \
  -o expander_test
  
 * 用法：
 *   ./expander_test                # 只运行功能测试
 *   ./expander_test --leak         # 用 valgrind 运行功能测试并检查内存泄露
 *
 * 注：--leak 模式需要系统已安装 valgrind。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "minishell.h"
#include "lexer.h"      /* remove_quotes_flag */
#include "expander.h"   /* expand_all, expander_str, ... */

/* 小工具：展开后“尝试去引号”
 * - 若 remove_quotes_flag 返回非 NULL：说明原串含引号 → 使用 clean
 * - 若返回 NULL：说明原串无引号     → 复制原串
 */
static char *expand_then_strip(t_minishell *m, const char *s)
{
	char *expanded = expand_all(m, s);
	char *clean;
	int had_q, qs, qd;

	if (!expanded) return NULL;
	clean = remove_quotes_flag(expanded, &had_q, &qs, &qd);
	if (!clean)
		clean = ft_strdup(expanded);
	free(expanded);
	return clean;
}

/* 辅助：打印测试结果并返回是否通过（1 = pass, 0 = fail） */
static int assert_eq(const char *testname, const char *got, const char *want)
{
	if (!got && !want)
	{
		printf("[PASS] %s : both NULL\n", testname);
		return 1;
	}
	if (!got || !want)
	{
		printf("[FAIL] %s : got=%s want=%s\n", testname, got ? got : "(null)", want ? want : "(null)");
		return 0;
	}
	if (strcmp(got, want) == 0)
	{
		printf("[PASS] %s : \"%s\"\n", testname, got);
		return 1;
	}
	else
	{
		printf("[FAIL] %s : got=\"%s\" want=\"%s\"\n", testname, got, want);
		return 0;
	}
}

/* 初始化 minishell 上下文（仅填充 expander 需要的字段） */
static void setup_minishell_env(t_minishell *m)
{
	static char *env[] = {
		"USER=alice",
		"HOME=/home/alice",
		"PATH=/usr/bin:/bin",
		NULL
	};
	memset(m, 0, sizeof(*m));
	m->envp = env;
	m->last_exit_status = 42; /* 用于 $? 测试 */
}

/* ---------- 单元测试 ---------- */

/* 测试 1：简单变量展开
 * 输入："Hello $USER" -> 期望 "Hello alice"
 */
static int test_expand_simple_var(t_minishell *m)
{
	const char *input = "Hello $USER";
	const char *want = "Hello alice";
	char *out = expand_all(m, input);
	int ok = assert_eq("expand_simple_var", out, want);
	free(out);
	return ok;
}

/* 测试 2：$? 展开
 * 输入："status:$?" -> 期望 "status:42"
 */
static int test_expand_question_mark(t_minishell *m)
{
	const char *input = "status:$?";
	const char *want = "status:42";
	char *out = expand_all(m, input);
	int ok = assert_eq("expand_question_mark", out, want);
	free(out);
	return ok;
}

/* 测试 3：双引号允许展开 + 去引号（使用 expand_then_strip 组合）
 * 输入 "\"Hello $USER\"" -> 期望 "Hello alice"
 */
static int test_expand_strip_on_quoted(t_minishell *m)
{
	const char *input = "\"Hello $USER\"";
	const char *want = "Hello alice";
	char *out = expand_then_strip(m, input);
	int ok = assert_eq("expand_strip_on_quoted", out, want);
	free(out);
	return ok;
}

/* 测试 4：expander_str 对带空格的目标去引号
 * 输入 "\"/tmp/my file\"" -> 期望 "/tmp/my file"
 */
static int test_expander_str_with_spaces(t_minishell *m)
{
	char *s = ft_strdup("\"/tmp/my file\"");
	const char *want = "/tmp/my file";
	char *out = expander_str(m, s);
	int ok = assert_eq("expander_str_with_spaces", out, want);
	free(out);
	return ok;
}

/* 测试 5：$<digit> 策略 -> 空展开（消费 2 字符）
 * 输入 "$1 and $2" -> 期望 " and "
 */
static int test_expand_digit_vars(t_minishell *m)
{
	const char *input = "$1 and $2";
	const char *want = " and ";
	char *out = expand_all(m, input);
	int ok = assert_eq("expand_digit_vars", out, want);
	free(out);
	return ok;
}

/* 测试 6：单引号内不展开（expand_all 会保留引号）
 * 输入 "'$HOME' and $HOME" -> 期望 "'$HOME' and /home/alice"
 */
static int test_single_quote_preserve(t_minishell *m)
{
	const char *input = "'$HOME' and $HOME";
	const char *want = "'$HOME' and /home/alice";
	char *out = expand_all(m, input);
	int ok = assert_eq("single_quote_preserve", out, want);
	free(out);
	return ok;
}

/* 运行所有单元测试，返回通过的个数 */
static int run_unit_tests(t_minishell *m)
{
	int passed = 0;
	int total = 0;

	total++; passed += test_expand_simple_var(m);
	total++; passed += test_expand_question_mark(m);
	total++; passed += test_expand_strip_on_quoted(m);
	total++; passed += test_expander_str_with_spaces(m);
	total++; passed += test_expand_digit_vars(m);
	total++; passed += test_single_quote_preserve(m);

	printf("\nSummary: passed %d / %d tests\n", passed, total);
	return (passed == total) ? 0 : 1;
}

/* ---------- valgrind 自我运行支持（用于检测内存泄露） ---------- */
static int run_self_under_valgrind(char *progpath, char **argv)
{
	pid_t pid;
	int status;
	char *vg = (char *)"valgrind";
	char *const vg_args[] = {
		(char *)"valgrind",
		(char *)"--leak-check=full",
		(char *)"--show-leak-kinds=all",
		(char *)"--error-exitcode=2",
		progpath,
		(char *)"--valgrind-run",
		NULL
	};

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return 127;
	}
	if (pid == 0)
	{
		execvp(vg, vg_args);
		perror("execvp valgrind");
		_exit(127);
	}
	if (waitpid(pid, &status, 0) < 0)
	{
		perror("waitpid");
		return 127;
	}
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	return 127;
}

/* ---------- 主程序 ---------- */
int main(int argc, char **argv)
{
	t_minishell m;
	int unit_ret;

	if (argc >= 2 && strcmp(argv[1], "--leak") == 0)
	{
		printf("Running tests under valgrind... (this requires valgrind installed)\n");
		fflush(stdout);
		int vg_ret = run_self_under_valgrind(argv[0], argv);
		if (vg_ret == 127)
			fprintf(stderr, "Failed to run valgrind. If valgrind is not installed, install it or run:\n"
				"  valgrind --leak-check=full --show-leak-kinds=all %s --valgrind-run\n", argv[0]);
		else if (vg_ret == 2)
			fprintf(stderr, "Valgrind detected memory errors or leaks (exit code 2).\n");
		else
			printf("Valgrind finished with exit code %d (0 means no leaks detected by valgrind).\n", vg_ret);
		return vg_ret;
	}

	if (argc >= 2 && strcmp(argv[1], "--valgrind-run") == 0)
	{
		setup_minishell_env(&m);
		unit_ret = run_unit_tests(&m);
		return unit_ret;
	}

	setup_minishell_env(&m);
	unit_ret = run_unit_tests(&m);
	return unit_ret;
}
