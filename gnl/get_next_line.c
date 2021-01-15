/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/17 17:10:32 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/25 11:28:34 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_strchr(const char *s, int c)
{
	char	*str;

	str = (char *)s;
	while (*str != c)
	{
		if (*str == '\0')
			return (NULL);
		str++;
	}
	return (str);
}

static int	ft_check_end(char *str)
{
	if (ft_strchr(str, '\n'))
	{
		ft_strcpy(str, ft_strchr(str, '\n') + 1);
		return (0);
	}
	return (1);
}

static int	ft_check_errors(int fd, char **line, char **str, char *buff)
{
	if (fd < 0 || BUFFER_SIZE < 1 || line == NULL || read(fd, buff, 0) < 0)
		return (-1);
	if (*str == NULL)
	{
		if ((*str = (char *)malloc(sizeof(char) * 1)) == NULL)
			return (-1);
		*str[0] = '\0';
	}
	return (1);
}

int			get_next_line(const int fd, char **line)
{
	static char	*str;
	char		*tmp;
	char		buff[BUFFER_SIZE + 1];
	int			size;

	if (ft_check_errors(fd, line, &str, buff) == -1)
		return (-1);
	while (ft_strchr(str, '\n') == NULL &&
			(size = read(fd, buff, BUFFER_SIZE)) > 0)
	{
		buff[size] = '\0';
		tmp = str;
		str = ft_strjoin(tmp, buff);
		free(tmp);
	}
	*line = ft_strndup(str, ft_linelen(str));
	if (ft_check_end(str) == 0)
		return (1);
	free(str);
	str = NULL;
	return (0);
}
