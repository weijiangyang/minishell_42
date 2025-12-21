/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 01:26:55 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/10 02:36:30 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	dispatch_format(char specifier, va_list *ap)
{
	if (specifier == 'c')
		return (print_char((char)va_arg(*ap, int)));
	else if (specifier == 's')
		return (print_string(va_arg(*ap, char *)));
	else if (specifier == 'd' || specifier == 'i')
		return (print_decimal(va_arg(*ap, int)));
	else if (specifier == 'u')
		return (print_unsigned(va_arg(*ap, unsigned int)));
	else if (specifier == 'x')
		return (print_hex(va_arg(*ap, unsigned int), 0));
	else if (specifier == 'X')
		return (print_hex(va_arg(*ap, unsigned int), 1));
	else if (specifier == 'p')
		return (print_pointer(va_arg(*ap, void *)));
	else if (specifier == '%')
		return (print_percent());
	return (-1);
}
