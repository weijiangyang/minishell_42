/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_hex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 23:23:16 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/10 01:28:40 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_hex(unsigned int n, int is_uppercase)
{
	int		len;
	char	*str;

	str = utoa_base(n, 16, is_uppercase);
	if (!str)
		return (-1);
	len = ft_strlen(str);
	if (write(1, str, len) < 0)
	{
		free(str);
		return (-1);
	}
	free(str);
	return (len);
}
