/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kalnajja <kalnajja@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 14:02:53 by kalnajja          #+#    #+#             */
/*   Updated: 2026/10/28 14:02:53 by kalnajja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*ft_error(char *buffer, char **stash)
{
	free(buffer);
	free(*stash);
	*stash = NULL;
	return (NULL);
}

static int	ft_update_stash(char **stash, char *buffer, ssize_t bytes_read)
{
	char	*temp;

	buffer[bytes_read] = '\0';
	temp = ft_strjoin(*stash, buffer);
	if (temp == NULL)
		return (0);
	free(*stash);
	*stash = temp;
	return (1);
}

static char	*ft_read_stash(int fd, char **stash)
{
	char	*buffer;
	ssize_t	bytes_read;

	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (ft_error(buffer, stash));
	bytes_read = 1;
	while (ft_strchr(*stash, '\n') == NULL && bytes_read != 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
			return (ft_error(buffer, stash));
		if (bytes_read > 0 && !ft_update_stash(stash, buffer, bytes_read))
			return (ft_error(buffer, stash));
	}
	free(buffer);
	return (*stash);
}

static char	*ft_get_result(char **stash)
{
	char	*line;
	char	*temp;

	line = ft_get_line(*stash);
	if (!line)
		return (ft_error(NULL, stash));
	temp = ft_new_stash(*stash);
	if (!temp)
	{
		free(line);
		return (ft_error(NULL, stash));
	}
	free(*stash);
	*stash = temp;
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*stash[1024];

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	if (ft_read_stash(fd, &stash[fd]) == NULL)
		return (NULL);
	if (!stash[fd] || stash[fd][0] == '\0')
	{
		free(stash[fd]);
		stash[fd] = NULL;
		return (NULL);
	}
	return (ft_get_result(&stash[fd]));
}
