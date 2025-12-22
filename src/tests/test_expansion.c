/* src/expansion/test.c
 *
 * Expander 单元测试 + 可选的 valgrind 泄露检测支持
 *
 * 建议编译（与原命令一致，可直接替换本文件）：
 *
 * gcc -g -O0 \
 *  -I include -I src/expansion -I src/lexer -I libft \
 *  src/expansion/expan_api.c \
 *  src/expansion/expan_env.c \
 *  src/expansion/expan_scan.c \
 *  src/expansion/expan_scan2.c \
 *  src/expansion/expan_utils.c \
 *  src/expansion/expan_list.c \
 *  src/expansion/test.c \
 *  src/expansion/eq_stub.c \
 *  src/lexer/lexer_remove_quotes.c \
 *  libft/libft.a \
 *  -o expander_test
 *
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
#include "expander.h"   /* expand_all, expander_str, expander_ast, ... */

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

/* ---------- 原有单元测试 ---------- */

static int test_expand_simple_var(t_minishell *m)
{
    const char *input = "Hello $USER";
    const char *want = "Hello alice";
    char *out = expand_all(m, input);
    int ok = assert_eq("expand_simple_var", out, want);
    free(out);
    return ok;
}

static int test_expand_question_mark(t_minishell *m)
{
    const char *input = "status:$?";
    const char *want = "status:42";
    char *out = expand_all(m, input);
    int ok = assert_eq("expand_question_mark", out, want);
    free(out);
    return ok;
}

static int test_expand_strip_on_quoted(t_minishell *m)
{
    const char *input = "\"Hello $USER\"";
    const char *want = "Hello alice";
    char *out = expand_then_strip(m, input);
    int ok = assert_eq("expand_strip_on_quoted", out, want);
    free(out);
    return ok;
}

static int test_expander_str_with_spaces(t_minishell *m)
{
    char *s = ft_strdup("\"/tmp/my file\"");
    const char *want = "/tmp/my file";
    char *out = expander_str(m, s);
    int ok = assert_eq("expander_str_with_spaces", out, want);
    free(out);
    return ok;
}

static int test_expand_digit_vars(t_minishell *m)
{
    const char *input = "$1 and $2";
    const char *want = " and ";
    char *out = expand_all(m, input);
    int ok = assert_eq("expand_digit_vars", out, want);
    free(out);
    return ok;
}

static int test_single_quote_preserve(t_minishell *m)
{
    const char *input = "'$HOME' and $HOME";
    const char *want = "'$HOME' and /home/alice";
    char *out = expand_all(m, input);
    int ok = assert_eq("single_quote_preserve", out, want);
    free(out);
    return ok;
}

/* ---------- 新增：基于 AST 的最小自测 ---------- */

/* 测 1：普通命令（非 export）—— argv[1] = "\"$USER\"" → 展开 + 去壳 = "alice" */
static int test_ast_cmd_basic(t_minishell *m)
{
    ast node;
    char **argv = (char **)malloc(sizeof(char *) * 3);
    int ok = 1;

    if (!argv) return 0;
    argv[0] = ft_strdup("echo");
    argv[1] = ft_strdup("\"$USER\"");
    argv[2] = NULL;

    memset(&node, 0, sizeof(node));
    node.type = NODE_CMD;
    node.argv = argv;            /* argv[1] 带引号，期望展开后去壳 */
    node.heredoc_delim = NULL;

    if (!expander_ast(m, &node)) ok = 0;
    if (ok) ok = (strcmp(node.argv[1], "alice") == 0);

    /* 释放 */
    free(node.argv[0]);
    free(node.argv[1]);
    free(node.argv);
    return assert_eq("ast_cmd_basic", ok ? "ok" : "fail", "ok");
}

/* 测 2：export 行为—— argv[1] = "\"$USER\"" → 仅展开，保留引号 = "\"alice\"" */
static int test_ast_cmd_export(t_minishell *m)
{
    ast node;
    char **argv = (char **)malloc(sizeof(char *) * 3);
    int ok = 1;
    const char *want = "\"alice\"";

    if (!argv) return 0;
    argv[0] = ft_strdup("export");
    argv[1] = ft_strdup("\"$USER\"");
    argv[2] = NULL;

    memset(&node, 0, sizeof(node));
    node.type = NODE_CMD;
    node.argv = argv;

    if (!expander_ast(m, &node)) ok = 0;
    if (ok) ok = (strcmp(node.argv[1], want) == 0);

    free(node.argv[0]);
    free(node.argv[1]);
    free(node.argv);
    return assert_eq("ast_cmd_export", ok ? "ok" : "fail", "ok");
}

/* 测 3：heredoc 定界符
 * 3.1 quoted 情况：heredoc_quoted=1，delimiter="'EOF'" → 期望去壳后 "EOF"
 * 3.2 unquoted 情况：heredoc_quoted=0，delimiter="\"$USER\"" → 展开+去壳 "alice"
 */
static int test_ast_heredoc_delim(t_minishell *m)
{
    ast node1, node2;
    int ok1 = 1, ok2 = 1;

    /* case 1: quoted delimiter → 只去壳 */
    memset(&node1, 0, sizeof(node1));
    node1.type = NODE_CMD;
    node1.heredoc_delim = ft_strdup("'EOF'");
    node1.heredoc_quoted = 1;
    if (!expander_ast(m, &node1)) ok1 = 0;
    if (ok1) ok1 = (strcmp(node1.heredoc_delim, "EOF") == 0);

    /* case 2: unquoted delimiter → 展开 + 去壳 */
    memset(&node2, 0, sizeof(node2));
    node2.type = NODE_CMD;
    node2.heredoc_delim = ft_strdup("\"$USER\"");
    node2.heredoc_quoted = 0;
    if (!expander_ast(m, &node2)) ok2 = 0;
    if (ok2) ok2 = (strcmp(node2.heredoc_delim, "alice") == 0);

    free(node1.heredoc_delim);
    free(node2.heredoc_delim);

    return assert_eq("ast_heredoc_delim",
                     (ok1 && ok2) ? "ok" : "fail", "ok");
}

/* 运行所有单元测试，返回通过的个数 */
static int run_unit_tests(t_minishell *m)
{
    int passed = 0;
    int total = 0;

    /* 原有用例 */
    total++; passed += test_expand_simple_var(m);
    total++; passed += test_expand_question_mark(m);
    total++; passed += test_expand_strip_on_quoted(m);
    total++; passed += test_expander_str_with_spaces(m);
    total++; passed += test_expand_digit_vars(m);
    total++; passed += test_single_quote_preserve(m);

    /* 新增 AST 入口用例 */
    total++; passed += test_ast_cmd_basic(m);
    total++; passed += test_ast_cmd_export(m);
    total++; passed += test_ast_heredoc_delim(m);

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
