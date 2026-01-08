# Signals（必须：ctrl-C / ctrl-D / ctrl-\ 行为要像 bash）

下面所有步骤都建议你同时开 2 个终端：
- 终端 A 跑 bash（作为参考）
- 终端 B 跑你的 minishell（对照）

## 1) 空提示符下按 ctrl-C
- bash：新起一行，打印新提示符
- minishell：应与 bash 一样（不退出，不打印 ^\ 等奇怪东西）

## 2) 空提示符下按 ctrl-D
- bash：退出 shell
- minishell：应退出

## 3) 空提示符下按 ctrl-\
- bash：什么也不做（通常不退出）
- minishell：应什么也不做

## 4) 前台运行程序时 ctrl-C
在两边都执行：
  cat
然后随便按几次回车，再按 ctrl-C
- bash：cat 被 SIGINT 中断，回到提示符
- minishell：应回到提示符，且 $? 应变成 130

## 5) 管道里 ctrl-C
执行：
  sleep 5 | sleep 5 | sleep 5
1 秒后按 ctrl-C
- bash：整条 pipeline 结束，回到提示符；$? 通常为 130
- minishell：对照 bash
