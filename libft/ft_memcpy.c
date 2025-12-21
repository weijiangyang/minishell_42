/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 14:03:58 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/10 02:05:44 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t				i;
	unsigned char		*str;
	const unsigned char	*s;

	i = 0;
	if (n == 0)
		return (dst);
	if (!dst || !src)
		return (NULL);
	str = (unsigned char *)dst;
	s = (const unsigned char *)src;
	while (i < n)
	{
		str[i] = s[i];
		i++;
	}
	return (dst);
}
