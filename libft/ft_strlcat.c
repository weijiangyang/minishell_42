/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 16:20:52 by yzhang2           #+#    #+#             */
/*   Updated: 2025/06/04 16:39:40 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	d;
	size_t	s;

	i = 0;
	if (!src)
		return (0);
	if (!dst && dstsize > 0)
		return (0);
	s = ft_strlen(src);
	if (dstsize == 0)
		return (s);
	d = ft_strlen(dst);
	if (dstsize <= d)
		return (s + dstsize);
	while (d + i < dstsize - 1 && src[i])
	{
		dst[d + i] = src[i];
		i++;
	}
	dst[d + i] = '\0';
	return (s + d);
}
