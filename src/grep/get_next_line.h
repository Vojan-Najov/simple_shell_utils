/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccartman <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 16:49:06 by ccartman          #+#    #+#             */
/*   Updated: 2021/05/18 20:43:23 by ccartman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif
# include <stddef.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <unistd.h>
# include <stdlib.h>

int		get_next_line(int fd, char **line);

size_t	ft_strlen(const char *s);

char	*ft_strcpy(char *dst, const char *src);

char	*ft_strchr(const char *s, int c);

char	*ft_strdup(const char *s);

char	*ft_strjoin_and_free_s1(char *s1, const char *s2);	

#endif
