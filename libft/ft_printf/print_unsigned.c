/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_unsigned.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 23:23:29 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/10 01:29:00 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_unsigned(unsigned int n)
{
	char	*str;
	int		len;

	str = utoa_base(n, 10, 0);
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
