/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:20:51 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/12 11:54:22 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <readline/readline.h>
# include <unistd.h>
# include <termios.h>
# include <signal.h>

typedef struct s_saved_signals
{
	struct sigaction	sigint;
	struct sigaction	sigquit;
	struct sigaction	sigtstp;
}	t_saved_signals;

void	sigint_prompt(int sig);
void	setup_prompt_signals(void);
void	setup_child_signals(void);
void	setup_parent_exec_signals(void);
void	setup_heredoc_signals(void);
void	save_signals(t_saved_signals *old);
void	ignore_heredoc_signals(void);
void	restore_signals(t_saved_signals *old);

#endif
