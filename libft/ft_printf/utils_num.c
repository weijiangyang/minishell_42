/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_num.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 23:07:34 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/10 02:55:27 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	count_digit(unsigned long long n, int base)
{
	int	len;

	len = 0;
	while (n != 0)
	{
		n /= (unsigned)base;
		len++;
	}
	return (len);
}

char	*utoa_base(unsigned long long n, int base, int is_uppercase)
{
	const char	*digit;
	char		*str;
	int			len;

	if (base < 2 || base > 16)
		return (NULL);
	digit = "0123456789abcdef";
	if (is_uppercase)
		digit = "0123456789ABCDEF";
	if (n == 0)
		return (ft_strdup("0"));
	len = count_digit(n, base);
	str = (char *)malloc(len + 1);
	if (!str)
		return (NULL);
	str[len] = '\0';
	while (len > 0)
	{
		str[len - 1] = digit[n % (unsigned)base];
		n /= (unsigned)base;
		len--;
	}
	return (str);
}
