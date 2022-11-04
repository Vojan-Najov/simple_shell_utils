/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccartman <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 17:20:37 by ccartman          #+#    #+#             */
/*   Updated: 2021/05/18 20:42:39 by ccartman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(const char *s)
{
	const char	*str;

	str = s;
	while (*str++)
		;
	return (--str - s);
}

char	*ft_strcpy(char *dst, char const *src)
{
	char	*d;

	d = dst;
	while (*src)
		*d++ = *src++;
	*d = '\0';
	return (dst);
}

char	*ft_strchr(const char *s, int c)
{
	char	smb;

	smb = (char) c;
	while (*s)
		if (*s++ == smb)
			return ((char *)--s);
	if (smb == '\0')
		return ((char *) s);
	else
		return (NULL);
}

char	*ft_strdup(const char *s1)
{
	char	*dup;
	size_t	len;

	len = ft_strlen(s1) + 1;
	dup = (char *) malloc(sizeof(*dup) * len);
	if (dup)
		ft_strcpy(dup, s1);
	return (dup);
}

char	*ft_strjoin_and_free_s1(char *s1, const char *s2)
{
	size_t	s1_len;
	size_t	s2_len;
	char	*str;

	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	str = (char *) malloc(sizeof(*str) * (s1_len + s2_len + 1));
	if (str)
	{
		ft_strcpy(str, s1);
		ft_strcpy(str + s1_len, s2);
	}
	free(s1);
	return (str);
}
