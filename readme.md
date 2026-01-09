```bash
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    readme.md                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: weiyang <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/09 12:05:00 by weiyang           #+#    #+#              #
#    Updated: 2026/01/09 12:05:05 by weiyang          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #
```

*This project has been created as part of the 42 curriculum by Yzhang2 and  Weiyang.*

# minishell<p align="center">
  <img src="logo.gif" width="800" alt="Minishell Logo">
</p>

## Description

**minishell** is a 42 School project whose goal is to implement a simplified Unix shell.
The project focuses on understanding how a shell works internally by recreating core
features such as command parsing, process creation, pipes, redirections, environment
variables, and signal handling.

The behavior of minishell aims to be as close as possible to **bash**, within the
constraints defined by the project subject.

---

## Instructions

### Compilation

To compile the project, run in the terminal:
```bash
    make 
```
This will generate the file executable :
```bash
    ./minishell
```
Launch minishell with:
```bash
    ./minishell
```
Example usage:
```bash
minishell$ echo "Hello, world"
Hello, world

```
### Features

* Interactive command prompt

* Execution of external programs using $PATH

* Built-in commands

* Pipes (|)

* Input , output redirections and heredoc:
```bash
<
>
>>
<< 
```
* Environment variable expansion ($VAR)

* Exit status expansion ($?)

* Single (') and double (") quotes

* Proper signal handling (Ctrl-C, Ctrl-D, Ctrl-\\, Ctrl-Z)


### Built-in Commands
```bash
| Command  | Description                   |
| -------- | ----------------------------- |
| `echo`   | Display a line of text        |
| `cd`     | Change the current directory  |
| `pwd`    | Print the working directory   |
| `export` | Set environment variables     |
| `unset`  | Remove environment variables  |
| `env`    | Display environment variables |
| `exit`   | Exit the shell                |
```

### Priority of commands  and File Descriptors (FD)

Our minishell correctly handles redirection and pipe priority:

Redirection takes precedence over pipes: If a command has both piped output and redirected file output (e.g., `ls | grep a > file`), `> file` overrides the pipe redirection, ensuring data flows to the final file.

FD leak protection: After each redirection and pipe operation, the program correctly closes any unused `pipe_fd`s, preventing system crashes due to file descriptor exhaustion.

### Signal Handling

minishell handles signals to mimic standard shell behavior and improve user experience.

Supported Signals
```bash
| Signal             | Default Behavior   | minishell Behavior                                             |
| ------------------ | ------------------ | -------------------------------------------------------------- |
| `SIGINT` (Ctrl+C)  | Terminates process | Interrupts foreground process, shell continues with new prompt |
| `SIGQUIT` (Ctrl+\)  | Core dump & exit   | Foreground process quits, shell ignores                        |
| `SIGTSTP` (Ctrl+Z) | Stops process      | Ignored by shell to avoid suspension                           |
```
Handling Strategy

* Shell (parent process):
  Captures SIGINT to print a new prompt; ignores SIGQUIT.
* Child processes:
  Restore default signal behavior so that foreground commands can be interrupted or quit normally.
* Heredoc Signal Handling
When reading a heredoc (<<), minishell handles signals carefully:
SIGINT (Ctrl+C): Interrupts the heredoc input immediately, discards the collected content, and returns control to the shell prompt.
SIGQUIT (Ctrl+\\): Ignored during heredoc input to avoid unwanted core dumps.
Child process: The heredoc reading typically runs in a separate process, which restores default signal behavior so Ctrl+C can terminate the input cleanly.
Key points:
    * Ensures the shell itself is not terminated while the user cancels a heredoc.
    * Prevents partial heredoc content from being passed to the command when interrupted.

### Error Handling

* Generic error return exit status 1
* Syntax errors return exit status 2

* Command not found returns 127

* Permission denied returns 126

* Error messages follow bash-style formatting
Example:
```bash
minishell: syntax error near unexpected token `newline'
```

### Limitations

* Logical operators (&&, ||) are not supported

* Wildcards (*) are not supported

* Subshells (( )) are not supported

## Resources
### Technical References

* Bash Manual
https://www.gnu.org/software/bash/manual/

* Advanced Programming in the UNIX Environment – W. Richard Stevens

* Linux man pages (man bash, man fork, man execve, man pipe, man signal)

### AI Usage Disclosure

* AI tools (such as ChatGPT and Google Gemini) were used for:

* Clarifying shell behavior by comparing it with bash

* Understanding edge cases related to parsing, executing, lexer, exit codes, and signals

* Improving documentation clarity (README formatting and wording)

* All implementation decisions and code were written, reviewed, and validated by the project authors.

## Authors
* Yzhang2
* Weiyang
