/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccartman <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 16:47:52 by ccartman          #+#    #+#             */
/*   Updated: 2021/06/04 15:01:00 by ccartman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	free_ptr(char **ptr);

static char	*check_buffer(char *buf, char **line);

static int	read_file_until_nl(int fd, char *buf, char **line);

int	get_next_line(int fd, char **line)
{
	static char	*buf;
	char		*ptr_nl;
	int			n_read;

	n_read = 0;
	if (!buf)
	{
		buf = (char *) malloc(sizeof(*buf) * (BUFFER_SIZE + 1));
		if (!buf)
			return (-1);
		*buf = '\0';
	}
	ptr_nl = check_buffer(buf, line);
	if (!ptr_nl && *line)
		n_read = read_file_until_nl(fd, buf, line);
	if (!*line || n_read == -1)
	{
		free_ptr(line);
		return (free_ptr(&buf));
	}
	if (ptr_nl || n_read > 0 || *buf)
		return (1);
	free_ptr(&buf);
	return (0);
}

static char	*check_buffer(char *buf, char **line)
{
	char	*ptr_nl;

	ptr_nl = ft_strchr(buf, '\n');
	if (ptr_nl)
	{
		*ptr_nl = '\0';
		*line = ft_strdup(buf);
		++ptr_nl;
		ft_strcpy(buf, ptr_nl);
	}
	else
	{
		*line = ft_strdup(buf);
		*buf = '\0';
	}
	return (ptr_nl);
}

static int	read_file_until_nl(int fd, char *buf, char **line)
{
	int		n_read;
	char	*ptr_nl;

	n_read = read(fd, buf, BUFFER_SIZE);
	while (n_read > 0)
	{
		buf[n_read] = '\0';
		ptr_nl = ft_strchr(buf, '\n');
		if (ptr_nl)
		{
			*ptr_nl = '\0';
			*line = ft_strjoin_and_free_s1(*line, buf);
			++ptr_nl;
			ft_strcpy(buf, ptr_nl);
			break ;
		}
		else
		{
			*line = ft_strjoin_and_free_s1(*line, buf);
			*buf = '\0';
		}
		n_read = read(fd, buf, BUFFER_SIZE);
	}
	return (n_read);
}

static int	free_ptr(char **ptr)
{
	if (*ptr)
		free(*ptr);
	*ptr = NULL;
	return (-1);
}
