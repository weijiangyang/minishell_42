/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 17:29:14 by yzhang2           #+#    #+#             */
/*   Updated: 2025/12/18 18:17:32 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

#include <errno.h>
#include <string.h>

void	ms_err_cmd_not_found(const char *cmd);
void	ms_err_exec(const char *name, int err);
void	ms_err_redir(const char *name, int err);
void	ms_perror(const char *ctx);

#endif
