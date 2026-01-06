/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 17:29:14 by yzhang2           #+#    #+#             */
/*   Updated: 2026/01/06 19:26:12 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "../libft/libft.h"

void	ms_put3(const char *a, const char *b, const char *c);
void	ms_err_cmd_not_found(const char *cmd);
void	ms_err_exec(const char *name, int err);
void	ms_err_redir(const char *name, int err);
void	ms_perror(const char *ctx);
void    ms_err_bad_interpreter(const char *cmd, const char *interp);
void    ms_err_syntax_unexpected(const char *tok);
void    ms_err_eof_quote(char q);

#endif
