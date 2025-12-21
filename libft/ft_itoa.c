/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 00:43:35 by yzhang2           #+#    #+#             */
/*   Updated: 2025/04/30 01:57:14 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_num(int n)
{
	int		count;
	long	num;

	count = 0;
	num = n;
	if (num < 0)
	{
		count += 1;
		num = -num;
	}
	if (num == 0)
		count = 1;
	while (num != 0)
	{
		count++;
		num = num / 10;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	int		count;
	char	*str;
	long	num;

	count = count_num(n);
	num = n;
	str = malloc(sizeof(char) * (count + 1));
	if (!str)
		return (NULL);
	str[count] = '\0';
	if (num < 0)
	{
		str[0] = '-';
		num = -num;
	}
	if (num == 0)
		str[0] = '0';
	while (num > 0)
	{
		count--;
		str[count] = num % 10 + '0';
		num = num / 10;
	}
	return (str);
}
