/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_pointer.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 23:23:23 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/10 15:03:54 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_pointer(void *ptr)
{
	unsigned long	addr;
	char			*hex;
	int				len;

	if (!ptr)
		return (write(1, "(nil)", 5));
	addr = (unsigned long)ptr;
	hex = utoa_base(addr, 16, 0);
	if (!hex)
		return (-1);
	len = ft_strlen(hex);
	if (write(1, "0x", 2) < 0 || write(1, hex, len) < 0)
	{
		free(hex);
		return (-1);
	}
	free(hex);
	return (len + 2);
}
