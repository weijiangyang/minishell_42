/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 23:23:03 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/10 01:20:57 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "../libft.h"
# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>

int		ft_printf(const char *format, ...);
int		dispatch_format(char specifier, va_list *ap);
int		print_char(char c);
int		ft_putchar(char c);
int		print_string(char *str);
int		print_decimal(int n);
char	*utoa_base(unsigned long long n, int base, int is_uppercase);
int		print_unsigned(unsigned int n);
int		print_percent(void);
int		print_pointer(void *ptr);
int		print_hex(unsigned int n, int is_uppercase);

#endif
