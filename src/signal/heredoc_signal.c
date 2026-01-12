/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_signal.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang<weiyang@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:19:41 by weiyang           #+#    #+#             */
/*   Updated: 2026/01/09 15:13:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

volatile sig_atomic_t	g_signal = 0;

void	save_signals(t_saved_signals *old)
{
	sigaction(SIGINT, NULL, &old->sigint);
	sigaction(SIGQUIT, NULL, &old->sigquit);
	sigaction(SIGTSTP, NULL, &old->sigtstp);
}

void	ignore_heredoc_signals(void)
{
	struct sigaction	sa_ignore;

	sa_ignore.sa_handler = SIG_IGN;
	sigemptyset(&sa_ignore.sa_mask);
	sa_ignore.sa_flags = 0;
	sigaction(SIGINT, &sa_ignore, NULL);
	sigaction(SIGQUIT, &sa_ignore, NULL);
	sigaction(SIGTSTP, &sa_ignore, NULL);
}

void	restore_signals(t_saved_signals *old)
{
	sigaction(SIGINT, &old->sigint, NULL);
	sigaction(SIGQUIT, &old->sigquit, NULL);
	sigaction(SIGTSTP, &old->sigtstp, NULL);
}

static void	sigint_heredoc(int sig)
{
	(void)sig;
	g_signal = SIGINT;
}

void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_heredoc;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGTSTP, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}
