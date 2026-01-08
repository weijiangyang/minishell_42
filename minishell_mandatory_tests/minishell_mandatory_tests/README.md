# minishell mandatory tests（已去除 bonus / 不在本套测试范围的内容）

这套测试只覆盖 minishell **mandatory** 里明确要求的功能：
- 引号、变量、$?
- 重定向：< > >> <<
- pipe
- builtins：echo(-n), cd, pwd, export, unset, env, exit
- PATH/绝对/相对路径执行

并且**不包含**：
- bonus：&& / || / 括号优先级 / wildcard *（题目 bonus）  
- 你特别要求移除的：分号 `;`、反斜杠 `\` 相关测试

> 题目原文：mandatory/bonus 的要求列表、以及 bonus 的内容请看 minishell.pdf。  
> 其中还明确：readline() 自己的泄漏不强制修，但你自己的代码不能漏。  

---

## 0) 放置位置

把整个 `minishell_mandatory_tests/` 文件夹放到你的项目根目录，例如：

```
minishell/
  minishell        (你的可执行文件，make 后生成)
  srcs/ ...
  minishell_mandatory_tests/
    run_tests.sh
    cases/
    manual/
```

---

## 1) 运行全部自动化测试

在项目根目录执行：

```
bash minishell_mandatory_tests/run_tests.sh
```

---

## 2) 只跑某一个模块（一个 cases 文件）

例如只跑重定向：

```
bash minishell_mandatory_tests/run_tests.sh minishell_mandatory_tests/cases/07_redirections.cases
```

---

## 3) 指定 minishell 可执行文件路径

默认会找 `./minishell`。如果你的可执行文件不在这里：

```
MINI=./path/to/minishell bash minishell_mandatory_tests/run_tests.sh
```

---

## 4) 输出怎么看

每条用例都会打印：
- PASS / FAIL
- 你的 minishell 与 bash 的 stdout/stderr 对比（FAIL 时会显示 diff）
- 退出码对比

同时会把原始输出保存到：
`minishell_mandatory_tests/logs/<module>/<testid>_mini.out` 等文件里。

---

## 5) 关于 prompt 差异

脚本按**非交互**方式喂输入，理论上 minishell 不应该打印 prompt。

如果你的 minishell 仍然会打印 prompt，导致 diff 全红：
- 先建议你按规范：只有在 `isatty(STDIN_FILENO)` 为真时才显示 prompt
- 或者你也可以设置一个正则来过滤 prompt 行（不推荐，但可用）：

```
MINI_PROMPT_REGEX='^minishell\$ ' bash minishell_mandatory_tests/run_tests.sh
```

---

## 6) 内存泄漏 / FD（手动检查建议）

题目允许 readline() 自身泄漏，但你自己的堆内存必须 free 干净。  
建议你在一些 case 上跑 valgrind：

```
valgrind --leak-check=full --track-fds=yes ./minishell < minishell_mandatory_tests/_one_input.txt
```

脚本运行时每个测试都会生成输入文件，你也可以直接拿某个 logs 目录里的 input 复跑。
