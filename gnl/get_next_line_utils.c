/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/24 15:57:59 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/25 09:16:38 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_strcpy(char *dest, char *src)
{
	size_t i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

int		ft_strlen(char const *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int		ft_linelen(char *str)
{
	int i;

	i = 0;
	while (str[i] && str[i] != '\n')
		i++;
	return (i);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	int		i;

	if (!(str = (char *)malloc(sizeof(char) *
					(ft_strlen(s1) + ft_strlen(s2) + 1))))
		return (NULL);
	i = 0;
	while (*s1)
	{
		str[i] = *s1;
		s1++;
		i++;
	}
	while (*s2)
	{
		str[i] = *s2;
		s2++;
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*ft_strndup(char *src, int size)
{
	int		i;
	char	*str;

	if (src == NULL)
		return (NULL);
	if ((str = malloc(sizeof(char) * size + 1)) == NULL)
		return (NULL);
	i = -1;
	while (++i < size)
		str[i] = src[i];
	str[i] = '\0';
	return (str);
}
