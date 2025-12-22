/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 15:20:51 by weiyang           #+#    #+#             */
/*   Updated: 2025/12/22 15:20:54 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_H
#define SIGNAL_H

#include <readline/readline.h>
#include <unistd.h>

void sigint_prompt(int sig);
void setup_prompt_signals(void);
void setup_child_signals(void);
void setup_parent_exec_signals(void);

#endif
