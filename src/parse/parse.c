/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yang <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 22:19:34 by yang              #+#    #+#             */
/*   Updated: 2025/10/31 22:19:38 by yang             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "parse.h"

token *head = NULL, *tail = NULL;
int n_pipes = 0;
// 创建一个 token 并返回
token *creat_new_token(tok_type ty, char *txt)
{
    token *t = malloc(sizeof(token));
    if (!t)
        return NULL;
    t->type = ty;
    if (txt)
        t->text = strdup(txt);
    else
        t->text = NULL;
    t->next = NULL;
    return (t);
}

void add_tok(tok_type ty, char *txt)
{

    token *t = creat_new_token(ty, txt);

    if (!t)
    {
        return;
    }
    if (head == NULL)
        head = tail = t;
    else
    {

        tail->next = t;
        tail = t;
    }
}
// 把输入字符串 str 分析成一个 token 链表，返回头指针
static token *tokenize(const char *str)
{
    const char *p = str;

    while (*p)
    {
        // 跳过空白
        if (isspace((unsigned char)*p))
        {
            p++;
            continue;
        }

        // 多字符运算符： "&&", "||", ">>", "<<"
        if (p[0] == '&' && p[1] == '&')
        {
            add_tok(TOK_AND, "&&");
            p += 2;
        }
        else if (p[0] == '|' && p[1] == '|')
        {
            add_tok(TOK_OR, "||");
            p += 2;
        }
        else if (p[0] == '>' && p[1] == '>')
        {
            add_tok(TOK_APPEND, ">>");
            p += 2;
        }
        else if (p[0] == '<' && p[1] == '<')
        {
            add_tok(TOK_HEREDOC, "<<");
            p += 2;
        }
        else
        {
            // 单字符 token
            char c = *p;
            if (c == '|')
            {
                add_tok(TOK_PIPE, "|");
                p++;
            }
            else if (c == '>')
            {
                add_tok(TOK_REDIR_OUT, ">");
                p++;
            }
            else if (c == '<')
            {
                add_tok(TOK_REDIR_IN, "<");
                p++;
            }
            else if (c == '(')
            {
                add_tok(TOK_LPAREN, "(");
                p++;
            }
            else if (c == ')')
            {
                add_tok(TOK_RPAREN, ")");
                p++;
            }
            else
            {
                // 普通 word：连续的非空白、非操作符字符
                const char *start = p;
                while (*p && !isspace((unsigned char)*p) && !(p[0] == '&' && p[1] == '&') && !(p[0] == '|' && p[1] == '|') && *p != '>' && *p != '<' && *p != '|' && *p != '(' && *p != ')')
                {
                    p++;
                }
                size_t len = p - start;
                char *buf = calloc(len + 1, 1);
                strncpy(buf, start, len);
                add_tok(TOK_WORD, buf);
                free(buf);
            }
        }
    }

    // 添加终结符
    add_tok(TOK_END, NULL);
    return head;
}

// 辅助函数
static token *peek_token(void)
{
    return g_cur;
}
// 得到当前 token，然后内部状态（g_cur）被更新到下一个 token，为下一次处理做准备
static token *next_token(void)
{
    token *t = g_cur;
    if (g_cur)
        g_cur = g_cur->next;
    return t;
}
// 验证当前 token 是否符合预期类型，然后消费（进到下一个 token）或报错
static token *expect_token(tok_type type)
{
    if (!g_cur || g_cur->type != type)
    {
        fprintf(stderr, "Syntax error : expected token type %d\n", type);
    }
    return (next_token());
}

static ast *parse_simple_cmd(void)
{
    token *t;
    // 子shell
    if (peek_token() && peek_token()->type == TOK_LPAREN)
    {
        // consume "()"
        next_token();
        ast *node = calloc(1, sizeof(ast));
        node->type = NODE_SUBSHELL;
        // 解析shell里的and/or表达式（或符合命令表达式）
        node->sub = parse_and_or();
        // expect ")"
        expect_token(TOK_RPAREN);
        return (node);
    }
    // 否则，命令节点
    ast *node = calloc(1, sizeof(ast));
    node->type = NODE_CMD;
    // 初始化argv 数组（预留空间）
    size_t argv_cap = 8;
    size_t argc = 0;
    node->argv = calloc(argv_cap, sizeof(char *));
    // 重定向前缀： 可能多个
    while (peek_token() &&
           (peek_token()->type == TOK_REDIR_IN ||
            peek_token()->type == TOK_REDIR_OUT ||
            peek_token()->type == TOK_APPEND ||
            peek_token()->type == TOK_HEREDOC))
    {
        token *redir = next_token();
        token *file = expect_token(TOK_WORD);
        switch (redir->type)
        {
        case TOK_REDIR_IN:
            node->redir_in = strdup(file->text);
            break;
        case TOK_REDIR_OUT:
            node->redir_out = strdup(file->text);
            break;
        case TOK_HEREDOC:
            node->heredoc_delim = strdup(file->text);
            break;
        default:
            break;
        }
    }
    // 接下来必须至少有一个word（命令或argument)
    if (!peek_token() || peek_token()->type != TOK_WORD)
    {
        // 语法错误
        fprintf(stderr, "Syntax error: expected command name\n");
        // 处理错误：free node 等
        return node;
    }

    // 首个WORD是命令名
    t = next_token();
    node->argv[argc++] = strdup(t->text);

    // 后续可能跟 WORD 或重定向
    while (peek_token() &&
           (peek_token()->type == TOK_WORD ||
            peek_token()->type == TOK_REDIR_IN ||
            peek_token()->type == TOK_REDIR_OUT ||
            peek_token()->type == TOK_APPEND ||
            peek_token()->type == TOK_HEREDOC))
    {
        if (peek_token()->type == TOK_WORD)
        {
            t = next_token();
            // 若argv 不够则扩容
            if (argc + 1 >= argv_cap)
            {
                argv_cap *= 2;
                node->argv = realloc(node->argv, argv_cap * sizeof(char *));
            }
            node->argv[argc++] = strdup(t->text);
        }
        else
        {
            // 重定向后缀
            token *redir = next_token();
            token *file = expect_token(TOK_WORD);
            switch (redir->type)
            {
            case TOK_REDIR_IN:
                free(node->redir_in);
                node->redir_in = strdup(file->text);
                break;
            case TOK_REDIR_OUT:
                free(node->redir_out);
                node->redir_out = strdup(file->text);
                break;
            case TOK_APPEND:
                free(node->redir_append);
                node->redir_append = strdup(file->text);
                break;
            case TOK_HEREDOC:
                free(node->heredoc_delim);
                node->heredoc_delim = strdup(file->text);
                break;
            default:
                break;
            }
        }
    }
    node->argv[argc] = NULL;
    return (node);
}

static ast *parse_pipeline(void)
{
    ast *left = parse_simple_cmd();
    while (peek_token() && peek_token()->type == TOK_PIPE)
    {
        
        next_token(); // 消费"|"
        ast *right = parse_simple_cmd();
        ast *node = calloc(1, sizeof(ast));
        node->type = NODE_PIPE;
        node->left = left;
        node->right = right;
        n_pipes++;
        left = node;
    }
    return (left);
}

static ast *parse_and_or(void)
{
    ast *left = parse_pipeline();
    while (peek_token() && (peek_token()->type == TOK_AND || peek_token()->type == TOK_OR))
    {
        token *op = next_token();
        ast *right = parse_pipeline();
        ast *node = calloc(1, sizeof(ast));
        node->type = (op->type == TOK_AND ? NODE_AND : NODE_OR);
        node->left = left;
        node->right = right;
        left = node;
    }
    return (left);
}

static ast *parse_cmdline(void)
{
    ast *root = parse_and_or();
    if (peek_token() && peek_token()->type != TOK_END)
    {
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n",
                peek_token()->type);
        exit(1);
    }
    return (root);
}

// ---------- AST / Token 内存释放 ----------
static void free_ast(ast *node)
{
    if (!node)
        return;
    switch (node->type)
    {
    case NODE_CMD:
        if (node->argv)
        {
            for (int i = 0; node->argv[i]; i++)
            {
                free(node->argv[i]);
            }
            free(node->argv);
        }
        free(node->redir_in);
        free(node->redir_out);
        free(node->redir_append);
        free(node->heredoc_delim);
        break;
    case NODE_PIPE:
    case NODE_AND:
    case NODE_OR:
        free_ast(node->left);
        free_ast(node->right);
        break;
    case NODE_SUBSHELL:
        free_ast(node->sub);
        break;
    }
    free(node);
}

static void free_tokens(token *tok)
{
    while (tok && tok->next)
    {
        token *nx = tok->next;
        if (tok->text)
            free(tok->text);
        free(tok);
        tok = nx;
    }
}

static void print_ast(ast *node, int indent)
{
    if (!node)
        return;
    for (int i = 0; i < indent; i++)
        putchar(' ');
    switch (node->type)
    {
    case NODE_CMD:
        printf("CMD");
        if (node->argv)
        {
            for (int i = 0; node->argv[i]; i++)
            {
                printf(" \"%s\"", node->argv[i]);
            }
        }
        if (node->redir_in)
            printf(" < %s", node->redir_in);
        if (node->redir_out)
            printf(" > %s", node->redir_out);
        if (node->redir_append)
            printf(" >> %s", node->redir_append);
        if (node->heredoc_delim)
            printf(" << %s", node->heredoc_delim);
        printf("\n");
        break;
    case NODE_PIPE:
        printf("PIPE\n");
        print_ast(node->left, indent + 2);
        print_ast(node->right, indent + 2);
        break;
    case NODE_AND:
        printf("AND\n");
        print_ast(node->left, indent + 2);
        print_ast(node->right, indent + 2);
        break;
    case NODE_OR:
        printf("OR\n");
        print_ast(node->left, indent + 2);
        print_ast(node->right, indent + 2);
        break;
    case NODE_SUBSHELL:
        printf("SUBSHELL\n");
        print_ast(node->sub, indent + 2);
        break;
    }
    
}

int main(int argc, char *argv[])
{
    char buf[1024];

    printf("Enter a shell command:\n");
    if (!fgets(buf, sizeof(buf), stdin))
    {
        return 0;
    }
    // 去掉行末换行符
    buf[strcspn(buf, "\n")] = '\0';

    token *tok = tokenize(buf);
    g_cur = tok;

    ast *root = parse_cmdline();
    printf("=== AST ===\n");
    print_ast(root, 0);

    // cleanup
    free_ast(root);
    free_tokens(tok);
    printf("n_pipes:%d\n", n_pipes);
    return (0);
}
