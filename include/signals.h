#ifndef SIGNAL_H
#define SIGNAL_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>
#include <termios.h>

void sigint_prompt(int sig);
void setup_prompt_signals(void);
void setup_child_signals(void);
void setup_parent_exec_signals(void);

#endif