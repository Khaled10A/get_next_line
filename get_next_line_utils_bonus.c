/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kalnajja <kalnajja@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 14:03:38 by kalnajja          #+#    #+#             */
/*   Updated: 2026/10/28 14:04:20 by kalnajja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static size_t	ft_strlen(char *str)
{
	size_t	i;

	i = 0;
	while (str && str[i])
		i++;
	return (i);
}

char	*ft_strjoin(char *stash, char *buffer)
{
	char	*joined;
	size_t	i;
	size_t	j;

	joined = malloc(ft_strlen(stash) + ft_strlen(buffer) + 1);
	if (!joined)
		return (NULL);
	i = 0;
	j = 0;
	while (stash && stash[i])
	{
		joined[i] = stash[i];
		i++;
	}
	while (buffer[j])
		joined[i++] = buffer[j++];
	joined[i] = '\0';
	return (joined);
}

char	*ft_get_line(char *stash)
{
	size_t	i;
	size_t	j;
	char	*line;

	if (!stash)
		return (NULL);
	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (stash[i] == '\n')
		i++;
	line = malloc(i + 1);
	if (!line)
		return (NULL);
	j = 0;
	while (j < i)
	{
		line[j] = stash[j];
		j++;
	}
	line[j] = '\0';
	return (line);
}

char	*ft_new_stash(char *stash)
{
	size_t	i;
	size_t	j;
	char	*new_stash;

	i = 0;
	if (!stash)
		return (NULL);
	while (stash[i] && stash[i] != '\n')
		i++;
	if (stash[i] == '\n')
		i++;
	j = 0;
	while (stash[i + j])
		j++;
	new_stash = malloc(j + 1);
	if (!new_stash)
		return (NULL);
	j = 0;
	while (stash[i + j])
	{
		new_stash[j] = stash[i + j];
		j++;
	}
	new_stash[j] = '\0';
	return (new_stash);
}

char	*ft_strchr(char *str, int c)
{
	size_t	i;

	i = 0;
	if (!str)
		return (NULL);
	while (str[i])
	{
		if ((unsigned char) str[i] == (unsigned char) c)
			return (&str[i]);
		i++;
	}
	if ((unsigned char) c == '\0')
		return (&str[i]);
	return (NULL);
}
