# History（必须：有 history 功能，但很难用纯脚本自动测）

1. 启动你的 minishell（交互模式）：
   ./minishell

2. 连续输入几条命令（例如）：
   echo one
   echo two
   echo three

3. 按 ↑（上箭头）：
   - 应该能回到上一条命令（echo three）
   - 再按 ↑ 应该依次回到 echo two / echo one

4. 按 ↓（下箭头）：
   - 应该能往“更新的命令”方向移动

5. 退出后再进入：
   - minishell 的 history 是否需要跨进程持久化？**题目没有要求**，一般不做也没问题。
